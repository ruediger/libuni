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

  namespace helper {
    extern
    std::uint16_t
    get_quick_check(codepoint_t cp);

    inline
    std::uint8_t
    is_allowed(std::uint16_t qc) {
      return qc;
    }

    inline
    std::uint8_t
    get_canonical_class(std::uint16_t qc) {
      return qc >> 8;
    }

    enum normalization_form{
      NFD  = 0x03u,
      NFKD = 0x0Cu,
      NFC  = 0x30u,
      NFKC = 0xC0u
    };

    template<normalization_form NF>
    quick_check_t
    is_allowed(std::uint16_t qc) {
      return static_cast<quick_check_t>(libuni::helper::is_allowed(qc) & NF);
    }
  }

  template<typename String, typename UTFTrait, helper::normalization_form Select>
  quick_check_t
  isNFX(String const &in) {
    std::uint8_t last_canonical_class = 0;
    quick_check_t result = Yes;
    typedef typename String::const_iterator iterator_t;
    iterator_t const end = in.end();
    iterator_t i = in.begin();
    codepoint_t cp;
    while(UTFTrait::next_codepoint(i, end, cp) == utf_ok) {
      std::uint16_t const qc = helper::get_quick_check(cp);
      std::uint8_t const canonical_class = helper::get_canonical_class(qc);
      if(last_canonical_class > canonical_class and canonical_class != 0) {
        return No;
      }

      quick_check_t const check = helper::is_allowed<Select>(qc);
      if(check == No) {
        return No;
      }
      else if(check == Maybe) {
        result = Maybe;
      }
      last_canonical_class = canonical_class;
    }
    return result;
  }

  template<typename String, typename UTFTrait = utf_trait<String>, helper::normalization_form Select>
  bool
  is_nfX(String const &in) {
    std::uint8_t last_canonical_class = 0;
    typedef typename String::const_iterator iterator_t;
    iterator_t const end = in.end();
    iterator_t i = in.begin();
    codepoint_t cp;
    while(UTFTrait::next_codepoint(i, end, cp) == utf_ok) {
      std::uint16_t const qc = helper::get_quick_check(cp);
      std::uint8_t const canonical_class = helper::get_canonical_class(qc);
      if(last_canonical_class > canonical_class and canonical_class != 0) {
        return false;
      }

      quick_check_t const check = helper::is_allowed<Select>(qc);
      if(check == No or check == Maybe) {
        return false;
      }
      last_canonical_class = canonical_class;
    }
    return true;
  }

  // Normalization Form D (NFD): Canonical Decomposition
  template<typename String>
  String toNFD(String const &in);

  template<typename String, typename UTFTrait = utf_trait<String>>
  quick_check_t
  isNFD(String const &in) {
    return isNFX<String, UTFTrait, helper::NFD>(in);
  }

  template<typename String, typename UTFTrait = utf_trait<String>>
  bool
  is_nfd(String const &in) {
    return is_nfX<String, UTFTrait, helper::NFD>(in);
  }

  // Normalization Form C (NFC): Canonical Decomposition, followed by Canonical Composition
  template<typename String>
  String toNFC(String const &in);

  template<typename String, typename UTFTrait = utf_trait<String>>
  quick_check_t
  isNFC(String const &in) {
    return isNFX<String, UTFTrait, helper::NFC>(in);
  }

  template<typename String, typename UTFTrait = utf_trait<String>>
  bool
  is_nfc(String const &in) {
    return is_nfX<String, UTFTrait, helper::NFC>(in);
  }

  // Normalization Form KD (NFKD): Compatibility Decomposition
  template<typename String>
  String toNFKD(String const &in);

  template<typename String, typename UTFTrait = utf_trait<String>>
  quick_check_t
  isNFKD(String const &in) {
    return isNFX<String, UTFTrait, helper::NFKD>(in);
  }

  template<typename String, typename UTFTrait = utf_trait<String>>
  bool
  is_nfkd(String const &in) {
    return is_nfX<String, UTFTrait, helper::NFKD>(in);
  }

  // Normalization Form KC (NFKC): Compatibility Decomposition, followed by Canonical Composition
  template<typename String>
  String toNFKC(String const &in);

  template<typename String, typename UTFTrait = utf_trait<String>>
  quick_check_t
  isNFKC(String const &in) {
    return isNFX<String, UTFTrait, helper::NFKC>(in);
  }

  template<typename String, typename UTFTrait = utf_trait<String>>
  bool
  is_nfkc(String const &in) {
    return is_nfX<String, UTFTrait, helper::NFKC>(in);
  }
}

#endif
