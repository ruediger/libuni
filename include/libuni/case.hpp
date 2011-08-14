/** case.hpp --- an implementation of Unicode's Defautl Case Algorithm (3.13)
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 * An implementation of Unicode's Default Case Algorithm (3.13).
 * Currently only simple/general (1-1) mapping is supported.
 */
#ifndef LIBUNI_CASE_HPP
#define LIBUNI_CASE_HPP

#include "codepoint.hpp"
#include "codepoint_string.hpp"
#include "utf.hpp"

namespace libuni {
  extern codepoint_t uppercase_mapping(codepoint_t cp);
  extern codepoint_t lowercase_mapping(codepoint_t cp);
  extern codepoint_t titlecase_mapping(codepoint_t cp);
  extern codepoint_t code_folding(codepoint_t cp);

  namespace helper {
    template<typename String, typename UTFTraits = utf_trait<String>, typename CaseMapping>
    String
    toXcase(String const &in, CaseMapping map) {
      typedef typename String::const_iterator iterator_t;
      iterator_t const end = in.end();
      iterator_t i = in.begin();
      codepoint_t cp;
      String ret;
      while(UTFTraits::next_codepoint(i, end, cp) == utf_ok) {
        UTFTraits::append(ret, map(cp));
      }
      return ret;
    }
  }

  template<typename String, typename UTFTraits = utf_trait<String>>
  String
  toUppercase(String const &in) {
    return helper::toXcase(in, uppercase_mapping);
  }

  template<typename String, typename UTFTraits = utf_trait<String>>
  String toLowercase(String const &in) {
    return helper::toXcase(in, lowercase_mapping);
  }

  template<typename String, typename UTFTraits = utf_trait<String>>
  String toTitlecase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  String toCasefold(String const &in);

  template<typename String, typename UTFTraits = utf_trait<String>>
  String toNFKC_Casefold(String const &in);

  namespace helper {
    extern
    bool
    is_uppercase(codepoint_t cp);

    extern
    bool
    is_lowercase(codepoint_t cp);

    template<typename String, typename UTFTraits = utf_trait<String>, typename isCase>
    bool
    isXcase(String const &in, isCase is_case) {
      typedef typename String::const_iterator iterator_t;
      iterator_t const end = in.end();
      iterator_t i = in.begin();
      codepoint_t cp;
      while(UTFTraits::next_codepoint(i, end, cp) == utf_ok) {
        if(not is_case(cp)) {
          return false;
        }
      }
      return true;
    }
  }

  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isUppercase(String const &in) {
    return helper::isXcase(in, helper::is_uppercase);
  }
  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isLowercase(String const &in) {
    return helper::isXcase(in, helper::is_lowercase);
  }

  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isTitlecase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isCasefold(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isCased(String const &in);
}

#endif
