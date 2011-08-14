// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>

#include <libuni/utf8.hpp>
#include <libuni/utf16.hpp>
#include <libuni/utf32.hpp>
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

BOOST_AUTO_TEST_CASE(test_isNFKD) {
  std::string str = "Ångstrom";
  BOOST_CHECK_EQUAL(libuni::isNFKD(str), libuni::No);
  BOOST_CHECK(not libuni::is_nfkd(str));
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
  BOOST_CHECK_NE(prefix, 0);
  BOOST_REQUIRE_EQUAL(end-begin, 2);
  BOOST_CHECK_EQUAL(begin[0], 0x6C);
  BOOST_CHECK_EQUAL(begin[1], 0xB7);

  // ΅
  BOOST_REQUIRE(libuni::helper::get_decomp_mapping(0x385, prefix, begin, end));
  BOOST_CHECK_EQUAL(prefix, 0);
  BOOST_REQUIRE_EQUAL(end-begin, 2);
  BOOST_CHECK_EQUAL(begin[0], 0xA8);
  BOOST_CHECK_EQUAL(begin[1], 0x301);

  // NO-BREAK
  BOOST_REQUIRE(libuni::helper::get_decomp_mapping(0xA0, prefix, begin, end));
  BOOST_CHECK_NE(prefix, 0);
  BOOST_REQUIRE_EQUAL(end-begin, 1);
  BOOST_CHECK_EQUAL(begin[0], 0x20);
}

BOOST_AUTO_TEST_CASE(test_toNFD) {
  std::string input = "UÜO";
  std::string output = libuni::toNFD(input);
  // CC88 is UTF-8 for 308 (COMBINING DIAERESIS)
  BOOST_CHECK_EQUAL(output, "UU\xCC\x88O");
}

#define TEST
#include "generate_two_stage_table.c++" // TODO move string_to_codepoint/parse_line to separate file

namespace {
  libuni::codepoint_string_t
  to_codepoint_string(std::string const &in) {
    libuni::codepoint_string_t str;
    str.reserve(in.size()/4);
    std::string::const_iterator beg = in.cbegin();
    std::string::const_iterator const end = in.cend();
    for(auto i = in.cbegin(); i != end; ++i) {
      if(*i == ' ') {
        if(beg != i) {
          str.push_back(string_to_codepoint(beg, i));
        }
        if(i+1 != end) {
          beg = i+1;
        }
      }
    }
    if(beg != end) {
      str.push_back(string_to_codepoint(beg, end));
    }
    return str;
  }
}

BOOST_AUTO_TEST_CASE(test_to_codepoint_string) {
  libuni::codepoint_string_t s = to_codepoint_string("FF 800 3C0");
  BOOST_REQUIRE_EQUAL(s.size(), 3);
  BOOST_CHECK_EQUAL(s[0], 0xFF);
  BOOST_CHECK_EQUAL(s[1], 0x800);
  BOOST_CHECK_EQUAL(s[2], 0x3C0);
}

BOOST_AUTO_TEST_CASE(test_toNFD_UCD) {
  std::ifstream in(UCD_PATH "NormalizationTest" UCD_VERSION ".txt");
  if(in) {
    for(boost::optional<std::vector<std::string>> line; in; line = parse_line(in)) {
      if(not line) {
        continue;
      }
      if(line->size() >= 5) {
        libuni::codepoint_string_t const c1 = to_codepoint_string((*line)[0]);
        libuni::codepoint_string_t const c2 = to_codepoint_string((*line)[1]);
        libuni::codepoint_string_t const c3 = to_codepoint_string((*line)[2]);
        libuni::codepoint_string_t const c4 = to_codepoint_string((*line)[3]);
        libuni::codepoint_string_t const c5 = to_codepoint_string((*line)[4]);

        BOOST_CHECK(c3 == libuni::toNFD(c1));
        BOOST_CHECK(c3 == libuni::toNFD(c2));
        BOOST_CHECK(c3 == libuni::toNFD(c3));
        BOOST_CHECK(c5 == libuni::toNFD(c4));
        BOOST_CHECK(c5 == libuni::toNFD(c5));

        BOOST_CHECK(c5 == libuni::toNFKD(c1));
        BOOST_CHECK(c5 == libuni::toNFKD(c2));
        BOOST_CHECK(c5 == libuni::toNFKD(c3));
        BOOST_CHECK(c5 == libuni::toNFKD(c4));
        BOOST_CHECK(c5 == libuni::toNFKD(c5));

        std::string const c1_u8 = libuni::utf8::from_codepoints(c1);
        std::string const c2_u8 = libuni::utf8::from_codepoints(c2);
        std::string const c3_u8 = libuni::utf8::from_codepoints(c3);
        std::string const c4_u8 = libuni::utf8::from_codepoints(c4);
        std::string const c5_u8 = libuni::utf8::from_codepoints(c5);
        BOOST_CHECK_EQUAL(c3_u8, libuni::toNFD(c1_u8));
        BOOST_CHECK_EQUAL(c3_u8, libuni::toNFD(c2_u8));
        BOOST_CHECK_EQUAL(c3_u8, libuni::toNFD(c3_u8));
        BOOST_CHECK_EQUAL(c5_u8, libuni::toNFD(c4_u8));
        BOOST_CHECK_EQUAL(c5_u8, libuni::toNFD(c5_u8));

        BOOST_CHECK_EQUAL(c5_u8, libuni::toNFKD(c1_u8));
        BOOST_CHECK_EQUAL(c5_u8, libuni::toNFKD(c2_u8));
        BOOST_CHECK_EQUAL(c5_u8, libuni::toNFKD(c3_u8));
        BOOST_CHECK_EQUAL(c5_u8, libuni::toNFKD(c4_u8));
        BOOST_CHECK_EQUAL(c5_u8, libuni::toNFKD(c5_u8));

        // TODO other NF* and UTF16
      }
    }
  }
}
