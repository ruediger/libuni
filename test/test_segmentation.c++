// -*- mode: c++; coding:utf-8; -*-

#include <boost/test/unit_test.hpp>
#include <libuni/segmentation.hpp>

BOOST_AUTO_TEST_CASE(test_get_word_breaks) {
  using namespace libuni;
  using namespace libuni::helper;
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
