#include <libuni/segmentation.hpp>
#include "generated/segmentation_database.hpp"

#include <cstddef>

namespace libuni {
namespace {
  enum break_value_shift {
    Word = 0,
    Sentence = 4,
    Grapheme = 8
  };
}

namespace helper {
  unsigned
  get_word_breaks(codepoint_t cp) {
    std::size_t const index = breaks_index[cp >> break_shift];
    std::uint16_t const info = breaks[(index << break_shift) + (cp & ( (1 << break_shift) - 1))];
    return static_cast<value_names>( (info >> Word) & 0xF );
  }
}

  unsigned const break_property::None = X_none_;
  unsigned const break_property::CR = CR_;
  unsigned const break_property::LF = LF_;
  unsigned const break_property::Newline = Newline_;
  unsigned const break_property::Extend = Extend_;
  unsigned const break_property::Format = Format_;
  unsigned const break_property::Katakana = Katakana_;
  unsigned const break_property::ALetter = ALetter_;
  unsigned const break_property::MidLetter = MidLetter_;
  unsigned const break_property::MidNum = MidNum_;
  unsigned const break_property::MidNumLet = MidNumLet_;
  unsigned const break_property::Numeric = Numeric_;
  unsigned const break_property::ExtendNumLet = ExtendNumLet_;
} // namespace libuni
