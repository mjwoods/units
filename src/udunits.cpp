#include "Rcpp.h"
#include "units.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP R_ut_init(CharacterVector path) {
  return XPtr<UnitSystem>(new UnitSystem(path));
}

// [[Rcpp::export]]
void R_ut_set_encoding(SEXP sys_, const std::string& enc_str) {
  XPtr<UnitSystem> sys(sys_);
  sys->set_encoding(enc_str);
}

// [[Rcpp::export]]
bool R_ut_is_parseable(SEXP sys_, const std::string& str) {
  XPtr<UnitSystem> sys(sys_);
  return sys->is_parseable(str);
}

// [[Rcpp::export]]
bool R_ut_are_convertible(SEXP sys_, const std::string& a, const std::string& b) {
  XPtr<UnitSystem> sys(sys_);
  return sys->are_convertible(a, b);
}

// [[Rcpp::export]]
NumericVector R_ut_convert_doubles(SEXP sys_, const std::string& from, 
                                   const std::string& to, NumericVector val) {
  XPtr<UnitSystem> sys(sys_);
  return sys->convert_doubles(from, to, val);
}

// [[Rcpp::export]]
void R_ut_new_dimensionless_unit(SEXP sys_, const std::string& name) {
  XPtr<UnitSystem> sys(sys_);
  sys->new_dimensionless_unit(name);
}

// [[Rcpp::export]]
void R_ut_scale(SEXP sys_, const std::string& from, const std::string& to, double factor) {
  XPtr<UnitSystem> sys(sys_);
  sys->scale(from, to, factor);
}

// [[Rcpp::export]]
void R_ut_offset(SEXP sys_, const std::string& from, const std::string& to, double factor) {
  XPtr<UnitSystem> sys(sys_);
  sys->offset(from, to, factor);
}

// [[Rcpp::export]]
std::string R_ut_get_symbol(SEXP sys_, const std::string& str) {
  XPtr<UnitSystem> sys(sys_);
  return sys->get_symbol(str);
}

// [[Rcpp::export]]
std::string R_ut_get_name(SEXP sys_, const std::string& str) {
  XPtr<UnitSystem> sys(sys_);
  return sys->get_name(str);
}

//// [[Rcpp::export]]
//XPtrUT R_ut_divide(SEXP numer, SEXP denom) {
//  return ut_wrap(ut_divide(ut_unwrap(numer), ut_unwrap(denom)));
//}

//// [[Rcpp::export]]
//XPtrUT R_ut_multiply(SEXP a, SEXP b) {
//  return ut_wrap(ut_multiply(ut_unwrap(a), ut_unwrap(b)));
//}

//// [[Rcpp::export]]
//XPtrUT R_ut_invert(SEXP a) {
//  return ut_wrap(ut_invert(ut_unwrap(a)));
//}

//// [[Rcpp::export]]
//XPtrUT R_ut_raise(SEXP a, IntegerVector i) {
//  if (i.length() != 1)
//    stop("i should have length 1");
//  return ut_wrap(ut_raise(ut_unwrap(a), i[0]));
//}

//// [[Rcpp::export]]
//XPtrUT R_ut_root(SEXP a, IntegerVector i) {
//  if (i.length() != 1)
//    stop("i should have length 1");
//  return ut_wrap(ut_root(ut_unwrap(a), i[0]));
//}

//// [[Rcpp::export]]
//XPtrUT R_ut_log(SEXP a, NumericVector base) {
//  if (base.length() != 1)
//    stop("base should have length 1");
//  if (base[0] <= 0)
//    stop("base should be positive");
//  return ut_wrap(ut_log(base[0], ut_unwrap(a)));
//}

// // [[Rcpp::export]]
// CharacterVector R_ut_format(SEXP p, bool names = false, bool definition = false, 
//                             bool ascii = false)
// {
//   int opt;
//   if (! ascii)
//     opt = enc;
//   else
//     opt = UT_ASCII;
//   if (names)
//     opt = opt | UT_NAMES;
//   if (definition)
//     opt = opt | UT_DEFINITION;
//   char buf[256];
//   ut_set_error_message_handler(ut_ignore);
//   int len = ut_format(ut_unwrap(p), buf, 256, opt);
//   ut_set_error_message_handler((ut_error_message_handler) r_error_fn);
//   if (len == -1) {
//     switch (ut_get_status()) {
//     case UT_BAD_ARG:
//     case UT_CANT_FORMAT:
//       handle_error("R_ut_format");
//       break;
//     default:;
//     }
//     buf[0] = '\0'; // "": dont' return rubbish
//   } else if (len == 256)
//     handle_error("buffer of 256 bytes too small!");
//   return CharacterVector::create(buf);
// }

// // https://github.com/r-quantities/units/issues/89#issuecomment-359251623
// // [[Rcpp::export]]
// XPtrUT R_ut_map_name_to_unit(CharacterVector name, SEXP inunit) {
//   ut_unit *unit = ut_unwrap(inunit);
//   if (ut_map_name_to_unit(name[0], enc, unit) != UT_SUCCESS)
//     handle_error("R_ut_map_name_to_unit");
//   return ut_wrap(unit);
// }
