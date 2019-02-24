ud_set_encoding = function(enc) {
  R_ut_set_encoding(sys, enc)
}

ud_is_parseable = function(u) {
  R_ut_is_parseable(sys, u)
}

ud_are_convertible = function(u1, u2) {
	R_ut_are_convertible(sys, u1, u2)
}

ud_new_dimensionless_unit <- function(u) {
  R_ut_new_dimensionless_unit(sys, u)
}

ud_scale <- function(from, to, const) {
  R_ut_scale(sys, from, to, 1.0 / as.double(const))
}

ud_offset <- function(from, to, const) {
  R_ut_offset(sys, from, to, as.double(const))
}

ud_get_symbol <- function(u) {
  sy = R_ut_get_symbol(sys, u)
  if (sy == "")
    R_ut_get_name(sys, u)
  else
    sy
}

ud_convert = function(value, from, to) {
	R_ut_convert_doubles(sys, from, to, value)
}
