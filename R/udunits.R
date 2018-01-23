ud_set_encoding = function(enc) {
  R_ut_set_encoding(sys, trimws(enc))
}

ud_is_parseable = function(u) {
  R_ut_is_parseable(sys, trimws(u))
}

ud_are_convertible = function(u1, u2) {
	R_ut_are_convertible(sys, trimws(u1), trimws(u2))
}

ud_new_dimensionless_unit <- function(u) {
  R_ut_new_dimensionless_unit(sys, trimws(u))
}

ud_scale <- function(from, to, const) {
  R_ut_scale(sys, trimws(from), trimws(to), 1.0 / as.double(const))
}

ud_offset <- function(from, to, const) {
  R_ut_offset(sys, trimws(from), trimws(to), as.double(const))
}

ud_get_symbol = function(u) {
  u <- trimws(u)
	res = try({R_ut_get_symbol(sys, u)}, silent=TRUE)
	if (inherits(res, "try-error"))
	  res = try({R_ut_get_symbol(sys, u)}, silent=TRUE)
	if (inherits(res, "try-error"))
		""
	else res
}

ud_convert = function(value, from, to) {
	R_ut_convert_doubles(sys, trimws(from), trimws(to), value)
}
