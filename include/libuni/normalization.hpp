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
#include "codepoint_string.hpp"
#include "utf8.hpp"

#include <cassert> // TODO

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

    extern
    bool
    get_decomp_mapping(codepoint_t cp, std::size_t &prefix, codepoint_t const *&begin, codepoint_t const *&end);
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

  namespace hangul {
    codepoint_t const SBase = 0xAC00;
    codepoint_t const LBase = 0x1100;
    codepoint_t const VBase = 0x1161;
    codepoint_t const TBase = 0x11A7;
    codepoint_t const LCount = 19;
    codepoint_t const VCount = 21;
    codepoint_t const TCount = 28;
    codepoint_t const NCount = VCount * TCount;
    codepoint_t const SCount = LCount * NCount;
  }

  template<typename String, typename UTFTrait = utf_trait<String>>
  String toNFD(String const &in) {
    if(is_nfd(in)) {
      return in;
    }

    codepoint_string_t tmp; // store codepoints before reorder
    tmp.reserve(in.size() + 10);
    codepoint_t stack[20];
    std::size_t stacksize = 0;
    typedef typename String::const_iterator iterator_t;
    iterator_t const end = in.end();
    iterator_t i = in.begin();
    codepoint_t cp;
    while(UTFTrait::next_codepoint(i, end, cp) == utf_ok) {
      stack[stacksize++] = cp;
      while(stacksize) {
        codepoint_t const code = stack[--stacksize];

        if(hangul::SBase <= code and code < hangul::SBase + hangul::SCount) { // Hangul Syllable Decomposition
          codepoint_t const SIndex = code - hangul::SBase;
          codepoint_t const L = hangul::LBase + SIndex / hangul::NCount;
          codepoint_t const V = hangul::VBase + (SIndex % hangul::NCount) / hangul::TCount;
          codepoint_t const T = hangul::TBase + SIndex % hangul::TCount;
          tmp.push_back(L);
          tmp.push_back(V);
          if(T != hangul::TBase) {
            tmp.push_back(T);
          }
          continue;
        }

        std::size_t prefix;
        codepoint_t const *begin = 0x0, *end = 0x0;
        if(helper::get_decomp_mapping(code, prefix, begin, end)) {
          assert(end - begin + stacksize <  sizeof(stack)/sizeof(stack[0]));
          while(end != begin) {
            stack[stacksize++] = *--end;
          }
        }
        else {
          tmp.push_back(code);
        }
      }
    }


    // Canonical Ordering Algorithm  (D109)
    std::uint8_t prev = helper::get_canonical_class(helper::get_quick_check(tmp[0]));
    codepoint_string_t::iterator sortbeg = tmp.begin() - 1;
    for(auto i = tmp.begin(); i != tmp.end(); ++i) {
      std::uint8_t cur = helper::get_canonical_class(helper::get_quick_check(*i));

      if(prev == 0 or cur == 0 or prev <= cur) {
        if(prev == 0 and cur != 0) {
          sortbeg = i - 1;
        }
        prev = cur;
      }
      else {
        codepoint_string_t::iterator j = i - 1;
        for(;;) {
          std::swap(*j, *(j + 1));
          --j;
          if(j == sortbeg) {
            break;
          }
          prev = helper::get_canonical_class(helper::get_quick_check(*j));
          if(prev <= cur) {
            break;
          }
        }
        prev = helper::get_canonical_class(helper::get_quick_check(*i));
      }
    }

    return UTFTrait::from_codepoints(tmp);
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
