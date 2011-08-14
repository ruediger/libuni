// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>
#include <libuni/case.hpp>

#include <libuni/utf8.hpp>
#include <libuni/utf16.hpp>
#include <libuni/utf32.hpp>

BOOST_AUTO_TEST_CASE(test_uppercase_mapping) {
  BOOST_CHECK_EQUAL(libuni::uppercase_mapping(0x61), 0x41); // a -> A
  BOOST_CHECK_EQUAL(libuni::uppercase_mapping(0xFC), 0xDC); // ü -> Ü
}

BOOST_AUTO_TEST_CASE(test_toUppercase) {
  BOOST_CHECK_EQUAL(libuni::toUppercase(std::string("Hello World")), "HELLO WORLD");
  BOOST_CHECK_EQUAL(libuni::toUppercase(std::string("Hёllö Wörld")), "HЁLLÖ WÖRLD");
}
