// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>

#include <libuni/utf8.hpp>
#include <cstring>

BOOST_AUTO_TEST_CASE(test_nextcodepoint_single) {
  libuni::char8_t const *strs[] = {
    (libuni::char8_t const *)"A",
    (libuni::char8_t const *)"ü",
    (libuni::char8_t const *)"大",
    (libuni::char8_t const *)"𐌸"
  };

  libuni::codepoint_t cps[] = {
    0x41,
    0xFC,
    0x5927,
    0x10338
  };

  std::size_t N = sizeof(strs)/sizeof(*strs);
  for(std::size_t i = 0; i < N ; ++i) {
    libuni::char8_t const *iter = strs[i];
    libuni::char8_t const *end = iter + std::strlen((char const*)iter);
    libuni::codepoint_t cp;
    libuni::utf_status s = libuni::next_codepoint(iter, end, cp);
    BOOST_CHECK_EQUAL(s, libuni::utf_ok);
    BOOST_CHECK_EQUAL(cp, cps[i]);
    BOOST_CHECK_EQUAL(iter, end);
  }
}

BOOST_AUTO_TEST_CASE(test_nextcodepoint_incomplete) {
  libuni::char8_t const str[] = { 0xFF, 0x00 };
  libuni::char8_t const *iter = str;
  libuni::char8_t const *const orig_iter = iter;
  libuni::char8_t const *const end = str + 1;
  libuni::codepoint_t cp;
  libuni::utf_status s = libuni::next_codepoint(iter, end, cp);
  BOOST_CHECK_EQUAL(s, libuni::incomplete_sequence);
  BOOST_CHECK_EQUAL(iter, orig_iter);
}

BOOST_AUTO_TEST_CASE(test_nextcodepoint_invalid) {
  libuni::char8_t const str[] = { 0xFF, 0x00 };
  libuni::char8_t const *iter = str;
  libuni::char8_t const *const orig_iter = iter;
  libuni::char8_t const *const end = str + 2;
  libuni::codepoint_t cp;
  libuni::utf_status s = libuni::next_codepoint(iter, end, cp);
  BOOST_CHECK_EQUAL(s, libuni::invalid_sequence);
  BOOST_CHECK_EQUAL(iter, orig_iter);
}
