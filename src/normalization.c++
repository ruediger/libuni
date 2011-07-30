#include <libuni/normalization.hpp>
#include "generated/normalization_database.hpp"

#include <cstdint>

namespace libuni {
  typedef std::uint32_t codepoint_t;

  enum quick_check_t {
    Yes,
    Maybe,
    No
  };

  inline 
  std::uint8_t
  is_allowed(codepoint_t cp) {
    std::size_t const index = quick_check_index[cp >> quick_check_shift];
    return quick_check[index];
  }

  inline
  quick_check_t
  is_allowed_nfd(codepoint_t cp) {
    return is_allowed(cp) & 0x3;
  }

  inline
  quick_check_t
  is_allowed_nfkd(codepoint_t cp) {
    return is_allowed(cp) & 0xC;
  }

  inline
  quick_check_t
  is_allowed_nfc(codepoint_t cp) {
    return is_allowed(cp) & 0x30;
  }

  inline
  quick_check_t
  is_allowed_nfkc(codepoint_t cp) {
    return is_allowed(cp) & 0xC0;
  }
}
