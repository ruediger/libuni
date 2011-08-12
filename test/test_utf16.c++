#include <boost/test/unit_test.hpp>
#include <libuni/utf16.hpp>

BOOST_AUTO_TEST_CASE(test_utf16_next_codepoint) {
  char16_t const str[] = { 0x004D, 0x0430, 0x4E8C, 0xD800, 0xDF02 };
  libuni::codepoint_t const cps[] = { 0x004D, 0x0430, 0x4E8C, 0x10302 };

  char16_t const *i = str;
  char16_t const *const end = str + sizeof(str)/sizeof(*str);
  libuni::codepoint_t cp;
  std::size_t n = 0;
  while(libuni::utf16::next_codepoint(i, end, cp) == libuni::utf_ok) {
    BOOST_CHECK_EQUAL(cp, cps[n++]);
    BOOST_CHECK_LE(n, sizeof(cps)/sizeof(*cps));
  }
  BOOST_CHECK_EQUAL(n, sizeof(cps)/sizeof(*cps));
}

BOOST_AUTO_TEST_CASE(test_utf16_bytes_required) {
  BOOST_CHECK_EQUAL(libuni::utf16::bytes_required(0x10302), 4);
  BOOST_CHECK_EQUAL(libuni::utf16::bytes_required(0x004D), 2);
}
