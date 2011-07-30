/** normalization.hpp --- an implementation of Unicode Normalization Forms (UAX#15)
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 * An implementation of Unicode Normalization Forms (UAX#15)
 * - latest: http://www.unicode.org/reports/tr15/
 * - at time of development: http://www.unicode.org/reports/tr15/tr15-33.html
 */
#ifndef LIBUNI_NORMALIZATION_HPP
#define LIBUNI_NORMALIZATION_HPP

#include "codepoint.hpp"
#include "utf8.hpp"

namespace libuni {
  enum quick_check_t {
    Yes,
    Maybe,
    No
  };

  quick_check_t
  is_allowed_nfd(codepoint_t cp);

  // Normalization Form D (NFD): Canonical Decomposition
  template<typename String>
  String toNFD(String const &in);

  template<typename String, typename UTFTrait = utf_trait<String>>
  quick_check_t
  isNFD(String const &in) {
    short last_canonical_class = 0;
    quick_check_t result = Yes;
    typedef typename String::const_iterator iterator_t;
    iterator_t const end = in.end();
    iterator_t i = in.begin();
    codepoint_t cp;
    while(UTFTrait::next_codepoint(i, end, cp) == utf_ok) {
#if 0 // TODO
      short const canonical_class = get_canonical_class(cp);
      if(last_canonical_class > canonical_class and canonical_class != 0) {
        return No;
      }
#endif
      quick_check_t const check = is_allowed_nfd(cp);
      if(check == No) {
        return No;
      }
      else if(check == Maybe) {
        result = Maybe;
      }
      //last_canonical_class = canonical_class;
    }
    return result;
  }

  template<typename String, typename UTFTrait = utf_trait<String>>
  bool
  is_nfd(String const &in) {
    short last_canonical_class = 0;
    typedef typename String::const_iterator iterator_t;
    iterator_t const end = in.end();
    iterator_t i = in.begin();
    codepoint_t cp;
    while(UTFTrait::next_codepoint(i, end, cp) == utf_ok) {
#if 0 // TODO
      short const canonical_class = get_canonical_class(cp);
      if(last_canonical_class > canonical_class and canonical_class != 0) {
        return false;
      }
#endif
      quick_check_t const check = is_allowed_nfd(cp);
      if(check == No or check == Maybe) {
        return false;
      }
      //last_canonical_class = canonical_class;
    }
    return true;
  }

  // Normalization Form C (NFC): Canonical Decomposition, followed by Canonical Composition
  template<typename String>
  String toNFC(String const &in);
  template<typename String>
  bool isNFC(String const &in);

  // Normalization Form KD (NFKD): Compatibility Decomposition
  template<typename String>
  String toNFKD(String const &in);
  template<typename String>
  bool isNFKD(String const &in);

  // Normalization Form KC (NFKC): Compatibility Decomposition, followed by Canonical Composition
  template<typename String>
  String toNFKC(String const &in);
  template<typename String>
  bool isNFKC(String const &in);
}

#endif
