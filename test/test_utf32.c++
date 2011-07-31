#include <boost/test/unit_test.hpp>
#include <libuni/utf32.hpp>

BOOST_AUTO_TEST_CASE(test_utf32_next_codepoint) {
  char32_t const str[] = { 0x41, 0xFC, 0x5927, 0x10338 };

  char32_t const *i = str;
  char32_t const *const end = str + sizeof(str)/sizeof(*str);
  libuni::codepoint_t cp;
  std::size_t n = 0;
  while(libuni::utf32::next_codepoint(i, end, cp) == libuni::utf_ok) {
    BOOST_CHECK_EQUAL(cp, str[n++]);
    BOOST_CHECK_LE(n, sizeof(str)/sizeof(*str));
  }
}
