#include <boost/test/unit_test.hpp>

#include <libuni/utf8.hpp>

BOOST_AUTO_TEST_CASE(test_nextcodepoint) {
  libuni::char8_t str[] = "ü";
  libuni::codepoint_t cp;
  libuni::char8_t *iter = str;
  libuni::char8_t *end = str + sizeof(str) - 1;
  libuni::utf_status s = libuni::next_codepoint(iter, end, cp);
  BOOST_CHECK_EQUAL(s, libuni::utf_ok);
  BOOST_CHECK_EQUAL(cp, 0x00FC);
  BOOST_CHECK_EQUAL(iter, end);
}
