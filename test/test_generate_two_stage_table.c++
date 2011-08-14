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

BOOST_AUTO_TEST_CASE(test_floor_log2) {
  BOOST_CHECK_EQUAL(floor_log2(10), 3);
  BOOST_CHECK_EQUAL(floor_log2(10lu), 3);
  BOOST_CHECK_EQUAL(floor_log2(1), 0);
}

BOOST_AUTO_TEST_CASE(test_container_max) {
  std::vector<int> v;
  v.push_back(10);
  BOOST_CHECK_EQUAL(container_max(v), 10);
  v.push_back(999);
  v.push_back(2);
  BOOST_CHECK_EQUAL(container_max(v), 999);
  v.push_back(10000);
  BOOST_CHECK_EQUAL(container_max(v), 10000);
}

BOOST_AUTO_TEST_CASE(test_getsize) {
  BOOST_CHECK_EQUAL(getsize(0xFF), 1);
  BOOST_CHECK_EQUAL(getsize(0xAAA), 2);
  BOOST_CHECK_EQUAL(getsize(0xFFFFF), 4);
}

BOOST_AUTO_TEST_CASE(test_gettype) {
  std::vector<int> v;
  v.push_back(0xFF);
  BOOST_CHECK_EQUAL(gettype(v), "std::uint8_t");
  v.push_back(0xFFF);
  BOOST_CHECK_EQUAL(gettype(v), "std::uint16_t");
  v.push_back(0xFFFFFF);
  BOOST_CHECK_EQUAL(gettype(v), "std::uint32_t");
}

BOOST_AUTO_TEST_CASE(test_insert_unique) {
  std::vector<std::string> v;
  std::size_t i = insert_unique(v, "hello");
  BOOST_CHECK_EQUAL(i, 0);
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK_EQUAL(v[0], "hello");
  i = insert_unique(v, "world");
  BOOST_CHECK_EQUAL(i, 1);
  BOOST_CHECK_EQUAL(v.size(), 2);
  BOOST_CHECK_EQUAL(v[0], "hello");
  BOOST_CHECK_EQUAL(v[1], "world");
  i = insert_unique(v, "hello");
  BOOST_CHECK_EQUAL(i, 0);
  BOOST_CHECK_EQUAL(v.size(), 2);
  BOOST_CHECK_EQUAL(v[0], "hello");
  BOOST_CHECK_EQUAL(v[1], "world");
}

BOOST_AUTO_TEST_CASE(test_handle_decomp_mapping) {
  std::vector<std::size_t> index(11, 0);
  std::vector<codepoint_t> map;
  std::vector<std::string> prefix;

  codepoint_t cp = 1;
  std::string str = "004C 004A";
  BOOST_REQUIRE(handle_decomp_mapping(cp, str, index, map, prefix));
  BOOST_CHECK_EQUAL(index[cp], 0);
  BOOST_REQUIRE_EQUAL(map.size(), 3);
  BOOST_CHECK_EQUAL(map[0] >> 8, 2);
  BOOST_CHECK_EQUAL(map[1], 0x4C);
  BOOST_CHECK_EQUAL(map[2], 0x4A);
  BOOST_CHECK_EQUAL(prefix.size(), 0);

  cp = 2;
  str = "<compat> 0064 017E";
  BOOST_REQUIRE(handle_decomp_mapping(cp, str, index, map, prefix));
  BOOST_CHECK_EQUAL(index[cp], 3);
  BOOST_REQUIRE_EQUAL(map.size(), 6);
  BOOST_CHECK_EQUAL(map[3], 2 << 8);
  BOOST_CHECK_EQUAL(map[4], 0x64);
  BOOST_CHECK_EQUAL(map[5], 0x17E);
  BOOST_REQUIRE_EQUAL(prefix.size(), 1);
  BOOST_CHECK_EQUAL(prefix[0], "compat");

  cp = 3;
  str = "004C 004A";
  BOOST_REQUIRE(handle_decomp_mapping(cp, str, index, map, prefix));
  BOOST_CHECK_EQUAL(index[cp], 0);
  BOOST_CHECK_EQUAL(map.size(), 6);
  BOOST_CHECK_EQUAL(map[0] >> 8, 2);
  BOOST_CHECK_EQUAL(map[1], 0x4C);
  BOOST_CHECK_EQUAL(map[2], 0x4A);

  cp = 4;
  str = "<noBreak> 0020";
  BOOST_REQUIRE(handle_decomp_mapping(cp, str, index, map, prefix));
  BOOST_CHECK_EQUAL(index[cp], 6);
  BOOST_REQUIRE_EQUAL(map.size(), 8);
  BOOST_CHECK_EQUAL(map[6], 1 << 8 | 1);
  BOOST_CHECK_EQUAL(map[7], 0x20);
  BOOST_REQUIRE_EQUAL(prefix.size(), 2);
  BOOST_CHECK_EQUAL(prefix[1], "noBreak");
}
