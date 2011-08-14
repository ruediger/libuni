/** case.hpp --- an implementation of Unicode's Defautl Case Algorithm (3.13)
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 * An implementation of Unicode's Default Case Algorithm (3.13)
 */
#ifndef LIBUNI_CASE_HPP
#define LIBUNI_CASE_HPP

#include "codepoint.hpp"
#include "codepoint_string.hpp"
#include "utf.hpp"

namespace libuni {
  codepoint_t lowercase_mapping(codepoint_t cp);
  codepoint_t titlecase_mapping(codepoint_t cp);
  codepoint_t uppercase_mapping(codepoint_t cp);
  codepoint_t code_folding(codepoint_t cp);

  template<typename String, typename UTFTraits = utf_trait<String>>
  String toUppercase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  String toLowercase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  String toTitlecase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  String toCasefold(String const &in);

  template<typename String, typename UTFTraits = utf_trait<String>>
  String toNFKC_Casefold(String const &in);

  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isUppercase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isLowercase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isTitlecase(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isCasefold(String const &in);
  template<typename String, typename UTFTraits = utf_trait<String>>
  bool isCased(String const &in);
}

#endif
