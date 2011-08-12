/** utf.hpp --- common stuff for UTF implementations
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 *
 * See utf8.hpp, utf16.hpp, utf32.hpp
 */
#ifndef LIBUNI_UTF_HPP
#define LIBUNI_UTF_HPP

#include "codepoint.hpp"

namespace libuni {
  enum utf_status { utf_ok, incomplete_sequence, invalid_sequence, end_of_string };

  template<typename String>
  struct utf_trait;
}

#endif
