#include <libuni/case.hpp>
#include "generated/case_database.hpp"

namespace libuni {
  codepoint_t
  uppercase_mapping(codepoint_t cp) {
    std::size_t const index = simple_uppercase_mapping_index[cp >> simple_uppercase_mapping_shift];
    codepoint_t const r = simple_uppercase_mapping[(index << simple_uppercase_mapping_shift) + (cp & ( (1 << simple_uppercase_mapping_shift) - 1))];
    return r == 0 ? cp : r;
  }

  bool
  helper::is_uppercase(codepoint_t cp) {
    std::size_t const index = simple_uppercase_mapping_index[cp >> simple_uppercase_mapping_shift];
    codepoint_t const r = simple_uppercase_mapping[(index << simple_uppercase_mapping_shift) + (cp & ( (1 << simple_uppercase_mapping_shift) - 1))];
    return r == 0;
  }

  codepoint_t
  lowercase_mapping(codepoint_t cp) {
    std::size_t const index = simple_lowercase_mapping_index[cp >> simple_lowercase_mapping_shift];
    codepoint_t const r = simple_lowercase_mapping[(index << simple_lowercase_mapping_shift) + (cp & ( (1 << simple_lowercase_mapping_shift) - 1))];
    return r == 0 ? cp : r;
  }

  bool
  helper::is_lowercase(codepoint_t cp) {
    std::size_t const index = simple_lowercase_mapping_index[cp >> simple_lowercase_mapping_shift];
    codepoint_t const r = simple_lowercase_mapping[(index << simple_lowercase_mapping_shift) + (cp & ( (1 << simple_lowercase_mapping_shift) - 1))];
    return r == 0;
  }

  codepoint_t
  titlecase_mapping(codepoint_t cp) {
    std::size_t const index = simple_titlecase_mapping_index[cp >> simple_titlecase_mapping_shift];
    codepoint_t const r = simple_titlecase_mapping[(index << simple_titlecase_mapping_shift) + (cp & ( (1 << simple_titlecase_mapping_shift) - 1))];
    return r == 0 ? uppercase_mapping(cp) : r;
  }
}
