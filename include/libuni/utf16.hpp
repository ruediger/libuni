/** utf16.hpp --- tools for UTF-16
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 *
 * See Ch.3.9
 */
#ifndef LIBUNI_UTF16_HPP
#define LIBUNI_UTF16_HPP

#include "utf.hpp"

namespace libuni {
  namespace utf16 {
    namespace helper {
      template<typename I, bool big2 = (sizeof(I) > 2)>
      struct cmp_max {
        static inline
        bool
        leFFFF(I i) {
          return i <= 0xFFFF;
        }
      };

      template<typename I>
      struct cmp_max<I, false> {
        static inline
        bool
        leFFFF(I) {
          return true;
        }
      };

      template<typename I>
      inline bool
      leFFFF(I i) {
        return cmp_max<I>::leFFFF(i);
      }
    }

    template<typename I>
    utf_status
    next_codepoint(I &i, I end, codepoint_t &cp) {
      switch(end - i) {
      default:
      case 2:
        if( (*i & 0xD800) == 0xD800 ) {
          I const first = i;
          ++i;
          if( (*i & 0xDC00) != 0xDC00 ) {
            i = first;
            return invalid_sequence;
          }
          cp =
            (codepoint_t(*first & 0x3C0) + 0x10000u) +
            ((*first & 0x3F) << 10) +
            (*i & 0x3FF);
        }
        else
      case 1:
        if(*i <= 0xD7FF or (0xE000 <= *i and helper::leFFFF(*i))) {
          cp = *i;
        }
        else {
          return incomplete_sequence;
        }
      case 0:
        break;
      }
      ++i;
      return utf_ok;
    }

    //// Bytes required to represent a codepoint
    inline
    std::size_t
    bytes_required(codepoint_t cp) {
      if(cp <= 0xD7FF or (0xE000 <= cp and cp <= 0xFFFF)) {
        return 2;
      }
      else if(0x10000 <= cp and cp <= 0x10FFFF) {
        return 4;
      }
      else {
        return 0;
      }
    }
  } // namespace utf16

  template<>
  struct utf_trait<std::u16string> {
    template<typename I>
    static
    utf_status
    next_codepoint(I &i, I end, codepoint_t &cp) {
      return utf16::next_codepoint(i, end, cp);
    }
  };
}

#endif
