#include <libuni/normalization.hpp>
#include "generated/normalization_database.hpp"

#include <cstdint>

namespace libuni {
  inline 
  std::uint8_t
  is_allowed(codepoint_t cp) {
    std::size_t const index = quick_check_index[cp >> quick_check_shift];
    return quick_check[index];
  }

  quick_check_t
  is_allowed_nfd(codepoint_t cp) {
    return static_cast<quick_check_t>(is_allowed(cp) & 0x3);
  }

  quick_check_t
  is_allowed_nfkd(codepoint_t cp) {
    return static_cast<quick_check_t>(is_allowed(cp) & 0xC);
  }

  quick_check_t
  is_allowed_nfc(codepoint_t cp) {
    return static_cast<quick_check_t>(is_allowed(cp) & 0x30);
  }

  quick_check_t
  is_allowed_nfkc(codepoint_t cp) {
    return static_cast<quick_check_t>(is_allowed(cp) & 0xC0);
  }
}
