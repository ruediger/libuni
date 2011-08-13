/** utf32.hpp --- tools for UTF-32
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 *
 * See Ch.3.9
 */
#ifndef LIBUNI_UTF32_HPP
#define LIBUNI_UTF32_HPP

#include "utf.hpp"

namespace libuni {
  namespace utf32 {
    template<typename I>
    utf_status
    next_codepoint(I &i, I end, codepoint_t &cp) {
      if(i == end) {
        return end_of_string;
      }
      else {
        cp = *i;
        ++i;
        return utf_ok;
      }
    }

    template<typename I>
    bool
    is_wellformed(I begin, I end) {
      for(I i = begin; i != end; ++i) {
        if((0xD800 <= *i and *i <= 0xDFFF) or 0x10FFFF < *i) {
          return false;
        }        
      }
      return true;
    }
  } // namespace utf32

  template<>
  struct utf_trait<std::u32string> {
    template<typename I>
    static
    utf_status
    next_codepoint(I &i, I end, codepoint_t &cp) {
      return utf32::next_codepoint(i, end, cp);
    }

    static inline
    std::u32string const&
    from_codepoints(std::u32string const &str) {
      return str;
    }
  };
}

#endif
