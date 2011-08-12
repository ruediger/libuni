// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>

#include <libuni/normalization.hpp>
#include <cstring>

BOOST_AUTO_TEST_CASE(test_is_allowed_nfd) {
  std::uint16_t qc = libuni::helper::get_quick_check(0x0374);
  BOOST_CHECK_EQUAL(libuni::helper::is_allowed<libuni::helper::NFD>(qc), libuni::No);
  qc = libuni::helper::get_quick_check(0x41);
  BOOST_CHECK_EQUAL(libuni::helper::is_allowed<libuni::helper::NFD>(qc), libuni::Yes);
  qc = libuni::helper::get_quick_check(0xC4);
  BOOST_CHECK_EQUAL(libuni::helper::is_allowed<libuni::helper::NFD>(qc), libuni::No);
  qc = libuni::helper::get_quick_check(0xC5);
  BOOST_CHECK_EQUAL(libuni::helper::is_allowed<libuni::helper::NFD>(qc), libuni::No);
}

BOOST_AUTO_TEST_CASE(test_isNFD) {
  std::string str = "Ångstrom";
  BOOST_CHECK_EQUAL(libuni::isNFD(str), libuni::No);
  BOOST_CHECK(not libuni::is_nfd(str));
}
