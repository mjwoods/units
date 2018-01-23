#' @import utils
#' @import stats
#' @importFrom Rcpp evalCpp
#' @useDynLib units
NULL

sys <- NULL

.onLoad = function(libname, pkgname) {
	sys <<- R_ut_init(.get_ud_xml_dir())
}

.onAttach <- function(libname, pkgname) {
    msg <- paste("udunits system database from", .get_ud_xml_dir(TRUE))
    packageStartupMessage(msg)
}
