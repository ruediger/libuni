#include <libuni/normalization.hpp>
#include "generated/normalization_database.hpp"

#include <cstdint>

namespace libuni { namespace helper {
  std::uint16_t
  get_quick_check(codepoint_t cp) {
    std::size_t const index = quick_check_index[cp >> quick_check_shift];
    return quick_check[(index << quick_check_shift) + (cp & ( (1 << quick_check_shift) - 1))];
  }

  bool
  get_decomp_mapping(codepoint_t cp, std::size_t &prefix, codepoint_t const *&begin, codepoint_t const *&end) {
    std::size_t const index1 = decomp_index1[cp >> decomp_shift];
    std::size_t const index = decomp_index2[ (index1 << decomp_shift) + (cp & ( (1 << decomp_shift) - 1)) ];

    assert(index < sizeof(decomp_map)/sizeof(decomp_map[0]));

    if(index == 0) {
      return false;
    }
    else {
      codepoint_t const info = decomp_map[index];
      prefix = info & 0xFF;
      std::size_t const len = (info >> 8) & 0xFF;
      begin = decomp_map + index + 1;
      end = begin + len;
      return true;
    }
  }
}}

