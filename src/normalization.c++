#include <libuni/normalization.hpp>
#include "generated/normalization_database.hpp"

#include <cstdint>

namespace libuni { namespace helper {
  std::uint16_t
  get_quick_check(codepoint_t cp) {
    std::size_t const index = quick_check_index[cp >> quick_check_shift];
    return quick_check[(index << quick_check_shift) + (cp & ( (1 << quick_check_shift) - 1))];
  }
}}

