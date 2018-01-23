#ifndef RCPP_UDUNITS2_H
#define RCPP_UDUNITS2_H

/*
 part of this was modified from: https://github.com/pacificclimate/Rudunits2
  
  (c) James Hiebert <hiebert@uvic.ca>
  Pacific Climate Impacts Consortium
  August, 16, 2010
  
  Functions to support the R interface to the udunits (API version 2) library
*/

#include <Rcpp.h>
#include <udunits2.h>

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

typedef Ptr<ut_unit, ut_free> Unit;
typedef Ptr<cv_converter, cv_free> Converter;

class UnitSystem : public Ptr<ut_system, ut_free_system> {
public:
  typedef Ptr<ut_system, ut_free_system> Parent;
  typedef std::map<std::string, Unit> Cache;
  
  UnitSystem(Rcpp::CharacterVector path) : Parent(NULL), enc(UT_UTF8) {
    ut_system* sys;
    ut_set_error_message_handler(ut_ignore);
    
    for (unsigned int i = 0; i < path.size(); i++) {
      if ((sys = ut_read_xml(path[i])) != NULL)
        break;
    }
    if (sys == NULL)
      sys = ut_read_xml(NULL);
    if (sys == NULL)
      Rcpp::stop("cannot instantiate a system of units");
    
    set(sys);
  }
  
  void set_encoding(const std::string& enc_str) {
    if (enc_str.compare("utf8") == 0)
      enc = UT_UTF8;
    else if (enc_str.compare("ascii") == 0)
      enc = UT_ASCII;
    else if (enc_str.compare("iso-8859-1") == 0 || enc_str.compare("latin1") == 0)
      enc = UT_LATIN1;
    else
      Rcpp::stop("Valid encoding string parameters are ('utf8'|'ascii'|'iso-8859-1','latin1')");
  }
  
  bool is_parseable(const std::string& str) {
    try { 
      resolve(str);
      return true;
    } catch (...) {}
    return false;
  }
  
  bool are_convertible(const std::string& a, const std::string& b) {
    return ut_are_convertible(resolve(a).get(), resolve(b).get());
  }
  
  Rcpp::NumericVector convert_doubles(const std::string& from, 
                                      const std::string& to, 
                                      Rcpp::NumericVector val)
  {
    Rcpp::NumericVector out(val.size());
    Converter cv(ut_get_converter(resolve(from).get(), resolve(to).get()));
    if (!cv_convert_doubles(cv.get(), &(val[0]), val.size(), &(out[0])))
      Rcpp::stop("cannot convert '%s' to '%s'", from, to);
    return out;
  }
  
  void new_dimensionless_unit(const std::string& name) {
    Unit u(ut_new_dimensionless_unit(get())); 
    if (ut_map_name_to_unit(name.c_str(), enc, u.get()) != UT_SUCCESS)
      Rcpp::stop("cannot create dimensionless unit '%s'", name);
  }
  
  void scale(const std::string& from, const std::string& to, double factor) {
    Unit u_new(ut_scale(factor, resolve(from).get()));
    if (ut_map_name_to_unit(to.c_str(), enc, u_new.get()) != UT_SUCCESS)
      Rcpp::stop("cannot create scaled unit '%s' from '%s'", to, from);
  }
  
  void offset(const std::string& from, const std::string& to, double factor) {
    Unit u_new(ut_offset(resolve(from).get(), factor));
    if (ut_map_name_to_unit(to.c_str(), enc, u_new.get()) != UT_SUCCESS)
      Rcpp::stop("cannot create offset unit '%s' from '%s'", to, from);
  }
  
  std::string get_symbol(const std::string& str) {
    Unit u = resolve(str);
    const char *s = ut_get_symbol(u.get(), enc);
    if (s == NULL)
      Rcpp::stop("cannot get symbol '%s'", str);
    return std::string(s);
  }
  
  std::string get_name(const std::string& str) {
    Unit u = resolve(str);
    const char *s = ut_get_name(u.get(), enc);
    if (s == NULL)
      Rcpp::stop("cannot get name '%s'", str);
    return std::string(s);
  }
  
private:
  ut_encoding enc;
  Cache cache;
  
  const Unit& resolve(const std::string& str) {
    Cache::const_iterator it = cache.find(str);
    if (it != cache.end())
      return it->second;
    ut_unit* u = ut_parse(get(), str.c_str(), enc);
    if (u == NULL)
      Rcpp::stop("cannot parse '%s'", str);
    return cache[str] = Unit(u);
  }
};

#endif
