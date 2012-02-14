// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>
#include <libuni/segmentation.hpp>

#include <fstream>
#include <vector>
#include <string>

BOOST_AUTO_TEST_CASE(test_get_word_breaks) {
  using namespace libuni;
  using namespace libuni::helper;
  BOOST_CHECK_EQUAL(get_word_breaks(0x01), break_property::None);
  BOOST_CHECK_EQUAL(get_word_breaks(0x0300), break_property::Extend);
  BOOST_CHECK_EQUAL(get_word_breaks(0x85), break_property::Newline);
  BOOST_CHECK_EQUAL(get_word_breaks(0x0D), break_property::CR);
  BOOST_CHECK_EQUAL(get_word_breaks(0x0A), break_property::LF);
  BOOST_CHECK_EQUAL(get_word_breaks(0x41), break_property::ALetter);
  BOOST_CHECK_EQUAL(get_word_breaks(0xFF70), break_property::Katakana);
  BOOST_CHECK_EQUAL(get_word_breaks(0x3A), break_property::MidLetter);
  BOOST_CHECK_EQUAL(get_word_breaks(0x2C), break_property::MidNum);
  BOOST_CHECK_EQUAL(get_word_breaks(0x2024), break_property::MidNumLet);
  BOOST_CHECK_EQUAL(get_word_breaks(0x30), break_property::Numeric);
  BOOST_CHECK_EQUAL(get_word_breaks(0xFF3F), break_property::ExtendNumLet);
}

namespace {
typedef std::vector<libuni::codepoint_t> codepoints;
typedef std::pair<codepoints, std::vector<codepoints>> breaks_t;

bool get_next_break_test(std::ifstream &in, breaks_t &breaks) {
  std::string line;
  while(std::getline(in, line)) {
    if(not line.empty() and line[0] != '#') {
      break;
    }
  }
  if(not in) {
    return false;
  }
  auto const end = line.cend();
  codepoints string;
  std::vector<codepoints> broken_up;
  codepoints current_break;
  string.reserve(line.size());
  for(auto i = line.cbegin(); i != end; ++i) {
    if(*i == '\xC3' and i+1 != end and *(i+1) == '\xB7') { // == ÷
      ++i;
      broken_up.push_back(current_break);
      current_break.clear();
    }
    else if(*i == '\xC3' and i+1 != end and *(i+1) == '\x97') { // == ×
      ++i;
    }
    else if(std::isxdigit(*i)) {
      std::size_t idx;
      libuni::codepoint_t const cp = std::stoul(std::string(i, end), &idx, 16);
      i += idx;
      string.push_back(cp);
      current_break.push_back(cp);
    }
    else if(*i == '#') {
      break;
    }
  }
  breaks = std::make_pair(string, broken_up);
  return true;
}

} // namespace

#include <iostream> // DEBUG

namespace std { // evil!
template<typename Char, class Traits, typename T, class Alloc>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits> &out, std::vector<T, Alloc> const &obj) {
  out << "{ ";
  bool first = true;
  for(auto const &x : obj) {
    if(not first) {
      out << ", ";
    }
    else {
      first = false;
    }
    out << x;
  }
  out << " }";
  return out;
}

template<typename Char, class Traits, typename Lhs, typename Rhs>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits> &out, std::pair<Lhs, Rhs> const &obj) {
  return out << "( " << obj.first << ", " << obj.second << " )";
}
}

BOOST_AUTO_TEST_CASE(test_WordBreakTest) {
  using namespace libuni;
  std::ifstream in(UCD_PATH "auxiliary/WordBreakTest.txt");
  breaks_t breaks;
  std::cout << libuni::helper::get_word_breaks(137) << std::endl;
  while(get_next_break_test(in, breaks)) {
    std::cout << breaks << std::endl; // DEBUG


    auto word_begin = breaks.first.cbegin();
    auto word_end = word_begin;
    auto end = breaks.first.cend();
    std::reverse(breaks.second.begin(), breaks.second.end());
    breaks.second.pop_back(); // remove sot rule (WB1)
    while(next_word(word_begin, word_end, end)) {
      codepoints word{word_begin, word_end};
      BOOST_REQUIRE(not breaks.second.empty());
      BOOST_CHECK_EQUAL(word, breaks.second.back());
      breaks.second.pop_back();
    }
    BOOST_CHECK(breaks.second.empty());
  }
}
