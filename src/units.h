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
  Ptr(T* ptr) : ptr(ptr) {}
  
  ~Ptr() { Finalize(ptr); }
  
  T* get() const { return ptr; }
  
  void set(T* p) { ptr = p; }
  
private:
  T* ptr;
};

typedef Ptr<ut_unit, ut_free> Unit;
typedef Ptr<cv_converter, cv_free> Converter;

class UnitSystem : public Ptr<ut_system, ut_free_system> {
public:
  typedef Ptr<ut_system, ut_free_system> Parent;
  
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
    this->set(sys);
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
    Unit u(ut_parse(this->get(), str.c_str(), enc));
    return u.get() != NULL;
  }
  
  bool are_convertible(const std::string& a, const std::string& b) {
    Unit u1(ut_parse(this->get(), a.c_str(), enc));
    Unit u2(ut_parse(this->get(), b.c_str(), enc));
    return ut_are_convertible(u1.get(), u2.get());
  }
  
  Rcpp::NumericVector convert_doubles(const std::string& from, 
                                      const std::string& to, 
                                      Rcpp::NumericVector val)
  {
    Rcpp::NumericVector out(val.size());
    Unit u1(ut_parse(this->get(), from.c_str(), enc));
    Unit u2(ut_parse(this->get(), to.c_str(), enc));
    Converter cv(ut_get_converter(u1.get(), u2.get()));
    if (!cv_convert_doubles(cv.get(), &(val[0]), val.size(), &(out[0])))
      Rcpp::stop("cannot convert '%s' to '%s'", from, to);
    return out;
  }
  
  void new_dimensionless_unit(const std::string& name) {
    Unit u = ut_new_dimensionless_unit(this->get()); 
    if (ut_map_name_to_unit(name.c_str(), enc, u.get()) != UT_SUCCESS)
      Rcpp::stop("cannot create dimensionless unit '%s'", name);
  }
  
  void scale(const std::string& from, const std::string& to, double factor) {
    Unit u_old(ut_parse(this->get(), from.c_str(), enc));
    Unit u_new(ut_scale(factor, u_old.get()));
    if (ut_map_name_to_unit(to.c_str(), enc, u_new.get()) != UT_SUCCESS)
      Rcpp::stop("cannot create scaled unit '%s' from '%s'", to, from);
  }
  
  void offset(const std::string& from, const std::string& to, double factor) {
    Unit u_old(ut_parse(this->get(), from.c_str(), enc));
    Unit u_new(ut_offset(u_old.get(), factor));
    if (ut_map_name_to_unit(to.c_str(), enc, u_new.get()) != UT_SUCCESS)
      Rcpp::stop("cannot create offset unit '%s' from '%s'", to, from);
  }
  
  std::string get_symbol(const std::string& str) {
    Unit u(ut_parse(this->get(), str.c_str(), enc));
    const char *s = ut_get_symbol(u.get(), enc);
    if (s == NULL)
      Rcpp::stop("cannot get symbol '%s'", str);
    return std::string(s);
  }
  
  std::string get_name(const std::string& str) {
    Unit u(ut_parse(this->get(), str.c_str(), enc));
    const char *s = ut_get_name(u.get(), enc);
    if (s == NULL)
      Rcpp::stop("cannot get name '%s'", str);
    return std::string(s);
  }
  
private:
  ut_encoding enc;
};

#endif
