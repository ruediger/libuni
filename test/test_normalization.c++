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

BOOST_AUTO_TEST_CASE(test_get_canonical_class) {
  std::uint16_t qc = libuni::helper::get_quick_check(0x05A1);
  BOOST_CHECK_EQUAL(libuni::helper::get_canonical_class(qc), 230);
  qc = libuni::helper::get_quick_check(0x05E9);
  BOOST_CHECK_EQUAL(libuni::helper::get_canonical_class(qc), 0);
}

BOOST_AUTO_TEST_CASE(test_isNFD) {
  std::string str = "Ångstrom";
  BOOST_CHECK_EQUAL(libuni::isNFD(str), libuni::No);
  BOOST_CHECK(not libuni::is_nfd(str));
}

BOOST_AUTO_TEST_CASE(test_get_decomp_mapping) {
  std::size_t prefix = 0;
  libuni::codepoint_t const *begin = 0x0, *end = 0x0;

  // U
  BOOST_CHECK(not libuni::helper::get_decomp_mapping(0x55, prefix, begin, end));

  // Ü
  BOOST_REQUIRE(libuni::helper::get_decomp_mapping(0xDC, prefix, begin, end));
  BOOST_CHECK_EQUAL(prefix, 0);
  BOOST_REQUIRE_EQUAL(end-begin, 2);
  BOOST_CHECK_EQUAL(*begin, 0x55);
  BOOST_CHECK_EQUAL(*(begin+1), 0x308);

  // ŀ
  BOOST_REQUIRE(libuni::helper::get_decomp_mapping(0x140, prefix, begin, end));
  BOOST_CHECK_EQUAL(prefix, 1);
  BOOST_REQUIRE_EQUAL(end-begin, 2);
  BOOST_CHECK_EQUAL(begin[0], 0x6C);
  BOOST_CHECK_EQUAL(begin[1], 0xB7);
}

BOOST_AUTO_TEST_CASE(test_toNFD) {
  std::string input = "UÜO";
  std::string output = libuni::toNFD(input);
  // CC88 is UTF-8 for 308 (COMBINING DIAERESIS)
  BOOST_CHECK_EQUAL(output, "UU\xCC\x88O");
  for(std::size_t i = 0; i < output.size(); ++i) {
    std::cerr << std::hex << (unsigned)output[i] << ' ' << (unsigned)"UU\xCC\x88O"[i] << std::endl;
  }
}
