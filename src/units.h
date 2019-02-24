#ifndef RCPP_UDUNITS2_H
#define RCPP_UDUNITS2_H

#include <Rcpp.h>
#include <udunits2.h>

using namespace Rcpp;

template <typename T, void Finalize(T*)>
class Ptr {
public:
  Ptr() : ptr(NULL), ref(new unsigned int(1)) {}
  
  Ptr(T* ptr) : ptr(ptr), ref(new unsigned int(1)) {}
  
  ~Ptr() { cleanup(); }
  
  Ptr(const Ptr& other) : ptr(other.ptr), ref(other.ref) { (*ref)++; }
  
  Ptr& operator=(const Ptr& other) {
    if(&other == this)
      return *this;
    cleanup();
    ptr = other.ptr;
    ref = other.ref;
    (*ref)++;
    return *this;
  }
  
  T* get() const { return ptr; }
  
protected:
  void set(T* p) { ptr = p; }
  
private:
  T* ptr;
  unsigned int* ref;
  
  void cleanup() {
    if (--(*ref)) return;
    if (ptr) Finalize(ptr);
    delete ref;
  }
};

using Unit = Ptr<ut_unit, ut_free>;
using Converter = Ptr<cv_converter, cv_free>;

class UnitSystem : public Ptr<ut_system, ut_free_system> {
public:
  using Parent = Ptr<ut_system, ut_free_system>;
  using Cache = std::unordered_map<const char*, Unit>;
  
  UnitSystem(CharacterVector path) : Parent(NULL), enc(UT_UTF8) {
    ut_system* sys;
    ut_set_error_message_handler(ut_ignore);
    
    for (unsigned int i = 0; i < path.size(); i++) {
      if ((sys = ut_read_xml(path[i])) != NULL)
        break;
    }
    if (sys == NULL)
      sys = ut_read_xml(NULL);
    if (sys == NULL)
      stop("cannot instantiate a system of units");
    
    set(sys);
  }
  
  void set_encoding(const char* enc_str) {
    if (std::strcmp(enc_str, "utf8") == 0)
      enc = UT_UTF8;
    else if (std::strcmp(enc_str, "ascii") == 0)
      enc = UT_ASCII;
    else if (std::strcmp(enc_str, "iso-8859-1") == 0 || std::strcmp(enc_str, "latin1") == 0)
      enc = UT_LATIN1;
    else
      stop("Valid encoding string parameters are ('utf8'|'ascii'|'iso-8859-1','latin1')");
  }
  
  bool is_parseable(char* str) {
    try {
      resolve(str);
      return true;
    } catch (...) {}
    return false;
  }
  
  bool are_convertible(char* a, char* b) {
    return ut_are_convertible(resolve(a).get(), resolve(b).get());
  }
  
  NumericVector convert_doubles(char* from, char* to, NumericVector val) {
    NumericVector out(val.size());
    Converter cv(ut_get_converter(resolve(from).get(), resolve(to).get()));
    if (!cv_convert_doubles(cv.get(), &(val[0]), val.size(), &(out[0])))
      stop("cannot convert '%s' to '%s'", from, to);
    return out;
  }
  
  void new_dimensionless_unit(char* name) {
    Unit u(ut_new_dimensionless_unit(get())); 
    if (ut_map_name_to_unit(name, enc, u.get()) != UT_SUCCESS)
      stop("cannot create dimensionless unit '%s'", name);
  }
  
  void scale(char* from, char* to, double factor) {
    Unit u_new(ut_scale(factor, resolve(from).get()));
    if (ut_map_name_to_unit(to, enc, u_new.get()) != UT_SUCCESS)
      stop("cannot create scaled unit '%s' from '%s'", to, from);
  }
  
  void offset(char* from, char* to, double factor) {
    Unit u_new(ut_offset(resolve(from).get(), factor));
    if (ut_map_name_to_unit(to, enc, u_new.get()) != UT_SUCCESS)
      stop("cannot create offset unit '%s' from '%s'", to, from);
  }
  
  const char* get_symbol(char* str) {
    Unit u = resolve(str);
    const char *s = ut_get_symbol(u.get(), enc);
    if (s == NULL)
      return "";
    return s;
  }
  
  const char* get_name(char* str) {
    Unit u = resolve(str);
    const char *s = ut_get_name(u.get(), enc);
    if (s == NULL)
      return "";
    return s;
  }
  
private:
  ut_encoding enc;
  Cache cache;
  
  const Unit& resolve(char* str) {
    char* str_trim = ut_trim(str, enc);
    auto it = cache.find(str_trim);
    if (it != cache.end())
      return it->second;
    ut_unit* u = ut_parse(get(), str_trim, enc);
    if (u == NULL)
      stop("cannot parse '%s'", str);
    return cache[str_trim] = Unit(u);
  }
};

#endif
