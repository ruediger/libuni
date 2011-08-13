// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>

#include <libuni/utf8.hpp>
#include <cstring>

BOOST_AUTO_TEST_CASE(test_utf8_next_codepoint_single) {
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
    libuni::utf_status s = libuni::utf8::next_codepoint(iter, end, cp);
    BOOST_CHECK_EQUAL(s, libuni::utf_ok);
    BOOST_CHECK_EQUAL(cp, cps[i]);
    BOOST_CHECK_EQUAL(iter, end);
  }
}

BOOST_AUTO_TEST_CASE(test_utf8_next_codepoint_incomplete) {
  libuni::char8_t const str[] = { 0xFF, 0x00 };
  libuni::char8_t const *iter = str;
  libuni::char8_t const *const orig_iter = iter;
  libuni::char8_t const *const end = str + 1;
  libuni::codepoint_t cp;
  libuni::utf_status s = libuni::utf8::next_codepoint(iter, end, cp);
  BOOST_CHECK_EQUAL(s, libuni::incomplete_sequence);
  BOOST_CHECK_EQUAL(iter, orig_iter);
}

BOOST_AUTO_TEST_CASE(test_utf8_next_codepoint_invalid) {
  libuni::char8_t const str[] = { 0xFF, 0x00 };
  libuni::char8_t const *iter = str;
  libuni::char8_t const *const orig_iter = iter;
  libuni::char8_t const *const end = str + 2;
  libuni::codepoint_t cp;
  libuni::utf_status s = libuni::utf8::next_codepoint(iter, end, cp);
  BOOST_CHECK_EQUAL(s, libuni::invalid_sequence);
  BOOST_CHECK_EQUAL(iter, orig_iter);
}

BOOST_AUTO_TEST_CASE(test_utf8_is_wellformed_false) {
  libuni::char8_t const str[] = {0xE0, 0x9F, 0x80};
  libuni::char8_t const *iter = str;
  libuni::char8_t const *const end = str + sizeof(str);
  BOOST_CHECK(!libuni::utf8::is_wellformed(iter, end));
}

BOOST_AUTO_TEST_CASE(test_utf8_is_wellformed_true) {
  libuni::char8_t const str[] = {0xF4, 0x80, 0x83, 0x92};
  libuni::char8_t const *iter = str;
  libuni::char8_t const *const end = str + sizeof(str);
  BOOST_CHECK(libuni::utf8::is_wellformed(iter, end));
}

BOOST_AUTO_TEST_CASE(test_utf8_bytes_required) {
  BOOST_CHECK_EQUAL(libuni::utf8::bytes_required(0x41), 1);    // A
  BOOST_CHECK_EQUAL(libuni::utf8::bytes_required(0xFC), 2);    // Ü
  BOOST_CHECK_EQUAL(libuni::utf8::bytes_required(0x5927), 3);  // 大
  BOOST_CHECK_EQUAL(libuni::utf8::bytes_required(0x10338), 4); // 𐌸
}

BOOST_AUTO_TEST_CASE(test_codepoint_to_utf8) {
  std::string s;
  libuni::utf8::codepoint_to_utf8(0x308, s);
  BOOST_REQUIRE_EQUAL(s.size(), 2);
  BOOST_CHECK_EQUAL(libuni::char8_t(s[0]), 0xCC);
  BOOST_CHECK_EQUAL(libuni::char8_t(s[1]), 0x88);

  s.clear();
  libuni::utf8::codepoint_to_utf8(0x41, s);
  BOOST_REQUIRE_EQUAL(s.size(), 1);
  BOOST_CHECK_EQUAL(s[0], 'A');

  s.clear();
  libuni::utf8::codepoint_to_utf8(0x5927, s);
  BOOST_REQUIRE_EQUAL(s.size(), sizeof("大")-1);
  BOOST_CHECK_EQUAL(s[0], "大"[0]);
  BOOST_CHECK_EQUAL(s[1], "大"[1]);
  BOOST_CHECK_EQUAL(s[2], "大"[2]);

  s.clear();
  libuni::utf8::codepoint_to_utf8(0xFC, s);
  BOOST_REQUIRE_EQUAL(s.size(), sizeof("ü")-1);
  BOOST_CHECK_EQUAL(s[0], "ü"[0]);
  BOOST_CHECK_EQUAL(s[1], "ü"[1]);

  s.clear();
  libuni::utf8::codepoint_to_utf8(0x10338, s);
  BOOST_REQUIRE_EQUAL(s.size(), sizeof("𐌸")-1);
  BOOST_CHECK_EQUAL(s[0], "𐌸"[0]);
  BOOST_CHECK_EQUAL(s[1], "𐌸"[1]);
  BOOST_CHECK_EQUAL(s[2], "𐌸"[2]);
  BOOST_CHECK_EQUAL(s[3], "𐌸"[3]);
}

BOOST_AUTO_TEST_CASE(test_utf8_from_codepoints) {
  libuni::codepoint_string_t s;
  s.push_back(0x41);
  s.push_back(0xFC);
  s.push_back(0x5927);
  s.push_back(0x10338);

  libuni::char8_t const str[] = "Aü大𐌸";

  std::string r = libuni::utf8::from_codepoints(s);
  BOOST_REQUIRE_EQUAL(r.size(), sizeof(str)/sizeof(str[0])-1);
  for(std::size_t i = 0; i < r.size(); ++i) {
    BOOST_CHECK_EQUAL(libuni::char8_t(r[i]), str[i]);
  }
}
