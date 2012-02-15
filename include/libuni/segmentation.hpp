/** segmentation.hpp --- an implementation of Unicode Text Segmentation (UAX#29)
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 * An implementation of Unicode Text Segmentation (UAX#29)
 */
#ifndef LIBUNI_SEGMENTATION_HPP
#define LIBUNI_SEGMENTATION_HPP

#include "codepoint.hpp"

namespace libuni {
  namespace break_property {
    extern unsigned const None;
    extern unsigned const CR;
    extern unsigned const LF;
    extern unsigned const Newline;
    extern unsigned const Extend;
    extern unsigned const Format;
    extern unsigned const Katakana;
    extern unsigned const ALetter;
    extern unsigned const MidLetter;
    extern unsigned const MidNum;
    extern unsigned const MidNumLet;
    extern unsigned const Numeric;
    extern unsigned const ExtendNumLet;
  }

  namespace helper {
    extern
    unsigned
    get_word_breaks(codepoint_t cp);
  }

  /** Sets word_begin/word_end to the beginning/end of the next word. Returns false on EOS.
   * Usage:
   *   iterator word_begin;
   *   iterator word_end = str.begin();
   *   iterator const end = str.end();
   *   while(next_word(word_begin, word_end, end)) {
   *     word = (word_begin, word_end);
   *   }
   */
  template<typename I>
  bool
  next_word(I &word_begin, I &word_end, I end) {
    using namespace break_property;

    if(word_end == end) {
      return false;
    }
    word_begin = word_end;
    unsigned s = helper::get_word_breaks(*word_end);

    for(;;) {
      unsigned const f = s;
      if(++word_end == end) { // WB2
        return true;
      }
      s = helper::get_word_breaks(*word_end);

      if(f == CR and s == LF) { // WB3
        continue;
      }
      else if( (f == CR or f == LF or f == Newline) or // WB3a
               (s == CR or s == LF or s == Newline) ) { // WB3b
        return true;
      }
      else if(s == Extend or s == Format) { // WB4
        s = f;
        continue;
      }
      else if(f == ALetter and s == ALetter) { // WB5
        continue;
      }
      else if( (f == Numeric and s == Numeric) or  //WB8
               (f == Numeric and s == ALetter) or  //WB9
               (f == ALetter and s == Numeric) ) { //WB10
        continue;
      }
      else if( f == ALetter and (s == MidLetter or s == MidNumLet) ) { // WB6/7
        I t = word_end;
        if(++t == end) {
          return true;
        }
        unsigned const n = helper::get_word_breaks(*t);
        if(n == ALetter) {
          s = n;
          word_end = t;
          continue;
        }
        else { // WB14
          return true;
        }
      }
      else if(f == Numeric and (s == MidNum or s == MidNumLet)) { // WB11/12
        I t = word_end;
        if(++t == end) {
          return true;
        }
        unsigned const n = helper::get_word_breaks(*t);
        if(n == Numeric) {
          s = n;
          word_end = t;
          continue;
        }
        else { // WB14
          return true;
        }
      }
      else if( f == Katakana and s == Katakana ) { // WB13
        continue;
      }
      else if( ((f == ALetter or f == Numeric or f == Katakana or f == ExtendNumLet) and s == ExtendNumLet) or // WB13a
               (f == ExtendNumLet and (s == ALetter or s == Numeric or s == Katakana)) ) { // WB13b
        continue;
      }
      else { // WB14
        return true;
      }
    }
  }
}

#endif
