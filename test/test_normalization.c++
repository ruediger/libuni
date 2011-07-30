// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>

#include <libuni/normalization.hpp>
#include <cstring>

BOOST_AUTO_TEST_CASE(test_is_allowed_nfd) {
  BOOST_CHECK_EQUAL(libuni::is_allowed_nfd(0x0374), libuni::No);
  BOOST_CHECK_EQUAL(libuni::is_allowed_nfd(0x41), libuni::Yes);
}

BOOST_AUTO_TEST_CASE(test_isNFD) {
  std::string str = "Ångstrom";
  BOOST_CHECK_EQUAL(libuni::isNFD(str), libuni::No);
  BOOST_CHECK(not libuni::is_nfd(str));
}
