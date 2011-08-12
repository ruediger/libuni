#include <boost/test/unit_test.hpp>

// This is a bit hacky.  Defining TEST deactivates the main function in the source file and exposes the internal functions.
#define TEST
#include "generate_two_stage_table.c++"

BOOST_AUTO_TEST_CASE(test_string_to_codepoint) {
  std::string a("AFFE");
  codepoint_t t = string_to_codepoint(a.begin(), a.end());
  BOOST_CHECK_EQUAL(t, 0xAFFE);
  a = "3a";
  t = string_to_codepoint(a.begin(), a.end());
  BOOST_CHECK_EQUAL(t, 0x3A);
}

BOOST_AUTO_TEST_CASE(test_creat_timmed_string) {
  std::string a(" ab ");
  std::string b(create_trimmed_string(a.begin(), a.end()));
  BOOST_CHECK_EQUAL(b, "ab");
}

BOOST_AUTO_TEST_CASE(test_parse_line) {
      std::stringstream ss;
    ss << "a;b;c # hello\nd;e;f\n";
    boost::optional<std::vector<std::string>> l = parse_line(ss);
    BOOST_CHECK(l);
    BOOST_CHECK_EQUAL(l->size(), 3);
    BOOST_CHECK_EQUAL((*l)[0], "a");
    BOOST_CHECK_EQUAL((*l)[1], "b");
    BOOST_CHECK_EQUAL((*l)[2], "c");
    boost::optional<std::vector<std::string>> k = parse_line(ss);
    BOOST_CHECK(k);
    BOOST_CHECK_EQUAL(k->size(), 3);
    BOOST_CHECK_EQUAL((*k)[0], "d");
    BOOST_CHECK_EQUAL((*k)[1], "e");
    BOOST_CHECK_EQUAL((*k)[2], "f");
}
