/** utf8.hpp --- tools for UTF-8
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 *
 */
#ifndef LIBUNI_UTF8_HPP
#define LIBUNI_UTF8_HPP

#include "codepoint.hpp"

namespace libuni {
  typedef unsigned char char8_t; // consistency with C++0x' char16_t/char32_t

  enum utf_status { utf_ok, incomplete_sequence, invalid_sequence };

  template<typename I>
  utf_status
  next_codepoint(I &i, I end, codepoint_t &cp) {
    switch(i - end) {
    default:
    case 4:
      if( (*i & 0xF0) == 0xF0 ) {
        I const first = i;
        ++i;
        if( (*i & 0x80) != 0x80 ) {
          i = first;
          return invalid_sequence;
        }
        I const second = i;
        ++i;
        if( (*i & 0x80) != 0x80 ) {
          i = first;
          return invalid_sequence;
        }
        I const third = i;
        ++i;
        if( (*i & 0x80) != 0x80 ) {
          i = first;
          return invalid_sequence;
        }
        cp = ( (codepoint_t(*first) << 18) +
               ((codepoint_t(*second) << 12) & 0x3F000) +
               ((codepoint_t(*third) << 6) & 0xFC0) +
               (*i & 0x3f) ) & 0x1FFFFF;
      }
      else
    case 3:
      if( (*i & 0xE0) == 0xE0 ) {
        I const first = i;
        ++i;
        if( (*i & 0x80) != 0x80 ) {
          i = first;
          return invalid_sequence;
        }
        I const second = i;
        ++i;
        if( (*i & 0x80) != 0x80 ) {
          i = first;
          return invalid_sequence;
        }
        cp = ( (codepoint_t(*first) << 12) +
               ((codepoint_t(*second) << 6) & 0xFC0) +
               (*i & 0x3f) ) & 0xFFFF;
      }
        else
    case 2:
      if( (*i & 0xC0) == 0xC0 ) {
        I const first = i;
        ++i;
        if( (*i & 0x80) != 0x80 ) {
          i = first;
          return invalid_sequence;
        }
        cp = ( (codepoint_t(*first) << 6) +
               (*i & 0x3F) ) & 0x7FF;
      }
      else
    case 1:
      if( (*i & 0x7F) == *i ) {
        cp = *i;
      }
      else {
        return incomplete_sequence;
      }
    case 0: // TODO?
      break;
    }
    ++i;
    return utf_ok;
  }
}

#endif
