context("udunits2")

test_that("udunits error messages", {
  expect_error(set_units(1:3, "qqq"), "qqq")
})

test_that("udunits low-level functions work", {
  expect_silent(units:::R_ut_get_dimensionless_unit_one(character(0)))
  a <- units:::R_ut_parse("m")
  b <- units:::R_ut_parse("g")
  expect_error(units:::R_convert_doubles(a, b, 1:10), "not convertible")

  u = units:::R_ut_offset("foo", "kg", -10)
  expect_equal(set_units(set_units(1, kg), foo), set_units(11, foo))
  remove_symbolic_unit("foo")

  expect_silent(units:::R_ut_divide(a, b))
  expect_silent(units:::R_ut_multiply(a, b))
  expect_silent(units:::R_ut_invert(a))
  expect_silent(sq <- units:::R_ut_raise(a, 2L))
  expect_error(units:::R_ut_raise(a, 2L:3L), "length 1")
  expect_silent(units:::R_ut_root(sq, 2L))
  expect_error(units:::R_ut_root(a, 2L:3L), "length 1")
  expect_silent(units:::R_ut_log(a, 10.0))
  expect_error(units:::R_ut_log(a, c(2, 10.0)), "length 1")
  expect_error(units:::R_ut_log(a, -10.0), "positive")
  expect_silent(units:::R_ut_format(a))
  expect_silent(units:::R_ut_format(a, ascii = TRUE))
  expect_silent(units:::R_ut_format(sq, names = TRUE))
  expect_silent(units:::R_ut_format(sq, definition = TRUE))
  expect_silent(units:::R_ut_set_encoding("ascii"))
  expect_silent(units:::R_ut_set_encoding("iso-8859-1"))
  expect_silent(units:::R_ut_set_encoding("latin1"))
  expect_silent(units:::R_ut_set_encoding("utf8"))
  expect_silent(ud_set_encoding("utf8"))
  expect_error(units:::R_ut_set_encoding("foo"))
  expect_error(units:::R_ut_get_symbol("foo"), "string unit representation contains unknown word")
  expect_error(units:::R_ut_get_name("foo"), "R_ut_get_name")
})

test_that("udunits database can be read", {
  expect_message(units:::.message_where_udunits_db(), "udunits")
  expect_message(valid_udunits(), "udunits")
  expect_message(valid_udunits_prefixes(), "udunits")
  # expect_identical(units:::.construct_ud_units(), ud_units) # NOT TRUE -- *ppv not in ud_units?
  expect_silent(units:::.construct_ud_units())
})
