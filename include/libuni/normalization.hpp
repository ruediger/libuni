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
#ifndef LIBUNI_UNICODE_CHARACTER_DATABASE_HPP
#define LIBUNI_UNICODE_CHARACTER_DATABASE_HPP

namespace libuni {
  // Normalization Form D (NFD): Canonical Decomposition
  template<typename String>
  String toNFD(String const &in);
  template<typename String>
  bool isNFD(String const &in);

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
