// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>
#include <libuni/utf_convert.hpp>

BOOST_AUTO_TEST_CASE(test_utf8_to_utf32) {
  std::string const in = "Aü大𐌸";
  libuni::codepoint_t const cps[] = {
    0x41,
    0xFC,
    0x5927,
    0x10338
  };

  std::u32string const s = libuni::utf8_to_utf32(in);
  BOOST_REQUIRE_EQUAL(s.size(), sizeof(cps)/sizeof(cps[0]));
  for(std::size_t i = 0; i < sizeof(cps)/sizeof(cps[0]); ++i) {
    BOOST_CHECK_EQUAL(s[i], cps[i]);
  }
}

BOOST_AUTO_TEST_CASE(test_utf32_to_utf8) {
  std::u32string u32 = {
    0x41,
    0xFC,
    0x5927,
    0x10338
  };
  std::string const u8 = "Aü大𐌸";
  BOOST_CHECK_EQUAL(u8, libuni::utf32_to_utf8(u32));
}
