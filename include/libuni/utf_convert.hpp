/** utf_convert.hpp --- tools to convert between UTF-8/16/32
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 *
 * See Ch.3.9, utf8.hpp, utf16.hpp, utf32.hpp
 */
#include "utf8.hpp"
#include "utf16.hpp"
#include "utf32.hpp"

namespace libuni {
  template<typename I>
  std::u32string utf8_to_utf32(I begin, I end) {
    typedef utf_trait<std::u32string> utf32traits;
    codepoint_t cp;
    std::u32string ret;
    while(utf8::next_codepoint(begin, end, cp) == utf_ok) {
      utf32traits::append(ret, cp);
    }
    return ret;
  }

  inline
  std::u32string
  utf8_to_utf32(std::string const &in) {
    return utf8_to_utf32(in.cbegin(), in.cend());
  }

  inline
  std::string
  utf32_to_utf8(std::u32string const &in) {
    typedef utf_trait<std::string> utf8traits;
    return utf8traits::from_codepoints(in);
  }
}
