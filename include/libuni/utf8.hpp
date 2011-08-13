/** utf8.hpp --- tools for UTF-8
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 *
 * See Ch.3.9
 */
#ifndef LIBUNI_UTF8_HPP
#define LIBUNI_UTF8_HPP

#include "utf.hpp"

#include <type_traits>
#include <string>

namespace libuni {
  typedef unsigned char char8_t; // consistency with C++0x' char16_t/char32_t

  namespace utf8 {

  template<typename I>
  utf_status
  next_codepoint(I &i, I end, codepoint_t &cp) {
    switch(end - i) {
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
      break;
    case 0:
      return end_of_string;
    }
    ++i;
    return utf_ok;
  }

  namespace helper {
    template<typename T, bool is_unsigned = std::is_unsigned<T>::value>
    struct is_larger_zero_ {
      static inline bool check(T t) {
        return 0x00 <= t;
      }
    };

    template<typename T>
    struct is_larger_zero_<T, true> {
      static inline bool check(T) {
        return true;
      }
    };

    template<typename T>
    bool is_larger_zero(T t) { // use this to avoid warnings
      return is_larger_zero_<T>::check(t);
    }
  }

  template<typename I>
  bool is_wellformed(I begin, I end) {
    for(I i = begin; i != end; ++i) {
      if(helper::is_larger_zero(*i) and *i <= 0x7F) {
      }
      else if(0xC2 <= *i and *i <= 0xDF) {
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
      }
      else if(*i == 0xE0) {
        ++i;
        if(i == end or not (0xA0 <= *i and *i <= 0xBF)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
      }
      else if( (0xE1 <= *i and *i <= 0xEC) or (0xEE < *i and *i < 0xEF) ) {
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
      }
      else if(*i == 0xED) {
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0x9F)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
      }
      else if(*i == 0xF0) {
        ++i;
        if(i == end or not (0x90 <= *i and *i <= 0xBF)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
      }
      else if(0xF1 < *i and *i < 0xF3) {
        ++i;
        if(i == end or not (0x90 <= *i and *i <= 0xBF)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
      }
      else if(*i == 0xF4) {
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0x8F)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
        ++i;
        if(i == end or not (0x80 <= *i and *i <= 0xBF)) {
          return false;
        }
      }
      else {
        return false;
      }
    }
    return true;
  }

  //// Bytes required to represent a codepoint
  inline
  std::size_t bytes_required(codepoint_t cp) {
    if(cp < 0x7F) {
      return 1;
    }
    else if(cp < 0x7FF) {
      return 2;
    }
    else if(cp < 0xFFFF) {
      return 3;
    }
    else {
      return 4;
    }
  }


  template<typename I>
  bool
  starts_with_bom(I begin, I end) {
    return
      (begin != end and *begin == 0xEF) and
      (++begin != end and *begin == 0xBB) and
      (++begin != end and *begin == 0xBF);
  }

  template<typename I>
  I
  skip_bom(I begin, I end) {
    I const tmp = begin;
    if((begin != end and *begin == 0xEF) and
       (++begin != end and *begin == 0xBB) and
       (++begin != end and *begin == 0xBF))
      return ++begin;
    else
      return tmp;
  }
  } // namespace utf8

  template<>
  struct utf_trait<std::string> { // assume std::string is UTF-8
    template<typename I>
    static
    utf_status
    next_codepoint(I &i, I end, codepoint_t &cp) {
      return utf8::next_codepoint(i, end, cp);
    }
  };
}

#endif
