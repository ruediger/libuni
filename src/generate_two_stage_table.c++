/** generate_two_stage_table.c++ --- Generates two stage tables from the UCD.
 *
 * Copyright (C) 2011 Rüdiger Sonderfeld <ruediger@c-plusplus.de>
 *
 * This file is part of libuni.
 *
 ** Commentary:
 * Loads the "Unicode Character Database" (UCD, see UAX#44) and generates two-stage tables in C++.
 *
 * This file MUST bootstrap and therefore SHOULD NOT rely on other libuni code (e.g., include headers from include/libuni).
 *
 * It takes the following compile-time parameters
 * - UCD_PATH :: the path to the directory containing the UCD files. MUST end with a /! [default: "UCD/"]
 * - UCD_VERSION :: suffix for UCD files. [default: ""]
 * - OUTDIR :: path to create the C++ files in. MUST end with a /! [default: "src/generated/"]
 *
 * The latest version of the Unicode Character Database can be found at
 * http://www.unicode.org/Public/UNIDATA/
 * See http://www.unicode.org/reports/tr44/ and http://www.unicode.org/ucd/
 */
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <boost/optional.hpp>
#include <boost/functional/hash.hpp>

#ifndef UCD_PATH
#define UCD_PATH "UCD/"
#endif
#ifndef UCD_VERSION
#define UCD_VERSION
#endif
#ifndef OUTDIR
#define OUTDIR "src/generated/"
#endif

namespace {
  typedef std::uint32_t codepoint_t;

  template<typename I>
  codepoint_t
  string_to_codepoint(I begin, I end) {
    codepoint_t ret = 0x00;
    for(; begin != end; ++begin) {
      if('0' <= *begin and *begin <= '9') {
        ret = ret * 0x10 + *begin - '0';
      }
      else if('A' <= *begin and *begin <= 'F') {
        ret = ret * 0x10 + *begin - 'A' + 0xa;
      }
      else if('a' <= *begin and *begin <= 'f') {
        ret = ret * 0x10 + *begin - 'a' + 0xa;
      }
    }
    return ret;
  }

  inline
  codepoint_t
  string_to_codepoint(std::string const &s) {
    return string_to_codepoint(s.cbegin(), s.cend());
  }

  template<typename I>
  std::string
  create_trimmed_string(I lhs, I rhs) {
    while(lhs != rhs and std::isspace(*lhs))
      ++lhs;
    if(lhs == rhs)
      return std::string();
    --rhs;
    while(rhs != lhs and std::isspace(*rhs))
      --rhs;
    ++rhs;
    return std::string(lhs, rhs);
  }

  /// Inserts str into v if v does not contain an equal string. Returns position of string in v.
  std::size_t
  insert_unique(std::vector<std::string> &v, std::string const &str) {
    assert(v.size() < 0xFF);
    auto i = std::find(v.begin(), v.end(), str);
    if(i == v.end()) {
      v.push_back(str);
      return v.size() - 1;
    }
    else {
      return i - v.begin();
    }
  }

  /// Handle decomp_mapping (see UAX#44 5.7.3): [<prefix>] codepoint codepoint ...
  bool
  handle_decomp_mapping(codepoint_t cp, std::string const &str,
                        std::vector<std::size_t> &decomp_index, std::vector<codepoint_t> &decomp_map, std::vector<std::string> &decomp_prefix)
  {
    std::vector<codepoint_t> ret(1, 0); // first codepoint is length + prefix
    std::string::const_iterator const end = str.cend();
    std::string::const_iterator i = str.cbegin();
    while(i != end and *i == ' ') {
      ++i;
    }
    if(i == end) {
      return false;
    }
    std::string::const_iterator last = i;

    // handle prefix
    if(*i == '<') {
      ++last;
      while(i != end and *i != '>') {
        ++i;
      }
      if(i == end) {
        return false;
      }
      else if(i != last) {
        ret[0] |= insert_unique(decomp_prefix, std::string(last, i));
        ++i;
        last = i;
      }
    }

    // handle mapping
    for(; i != end; ++i) {
      if(*i == ' ') {
        if(i != last) {
          ret.push_back(string_to_codepoint(last, i));
        }
        while(i != end and *i == ' ') {
          ++i;
        }
        last = i;
        if(i == end) {
          break;
        }
      }
    }
    if(last != end) {
      ret.push_back(string_to_codepoint(last, end));
    }

    if(ret.size() <= 1) {
      return false;
    }

    assert(ret.size() - 1 < 0xFF);
    ret[0] |= (ret.size() - 1) << 8;

    auto j = std::search(decomp_map.cbegin(), decomp_map.cend(),
                         ret.cbegin(), ret.cend());
    if(j == decomp_map.cend()) {
      decomp_index[cp] = decomp_map.size();
      decomp_map.insert(decomp_map.end(), ret.cbegin(), ret.cend());
    }
    else {
      decomp_index[cp] = j - decomp_map.cbegin();
    }
    return true;
  }

  // Data files are in UTF-8 but non-ASCII characters only in comments (UAX#44: 4.29)
  boost::optional<std::vector<std::string>>
  parse_line(std::istream &in) {
    std::string line;
    if(!std::getline(in, line) or line.empty() or line[0] == '#') {
      return boost::none;
    }
    std::string::const_iterator i = line.begin();
    std::string::const_iterator const end = line.end();
    std::vector<std::string> properties;
    for(std::string::const_iterator from = i;; ++i) {
      if(i == end) {
        if(from != end) {
          properties.push_back(create_trimmed_string(from, end));
        }
        break;
      }
      else if(*i == ';') {
        properties.push_back(create_trimmed_string(from, i));
        from = i;
        ++from;
      }
      else if(*i == '#') { // comment
        std::string const s = create_trimmed_string(from, i);
        if(not s.empty()) {
          properties.push_back(s);
        }
        break;
      }
    }
    return properties;
  }

  template<typename Int>
  Int
  floor_log2(Int i) {
    Int ret(0);
    while(i >>= 1) {
      ++ret;
    }
    return ret;
  }

  template<typename Cont>
  typename Cont::value_type
  container_max(Cont const &ct) {
    auto end = ct.end();
    typename Cont::value_type m;
    for(auto i = ct.begin(); i != end; ++i) {
      m = std::max(m, *i);
    }
    return m;
  }

  template<typename T>
  struct getsize_ {
    static
    std::size_t
    getsize(T t) {
      if(t <= 0xFF)
        return 1;
      else if(t <= 0xFFFF)
        return 2;
      else
        return 4;
    }
  };

  template<>
  struct getsize_<std::uint16_t> {
    static
    std::size_t
    getsize(std::uint16_t t) {
      if(t <= 0xFF)
        return 1;
      else
        return 2;
    }
  };

  template<>
  struct getsize_<std::uint8_t> {
    static
    std::size_t
    getsize(std::uint8_t) {
      return 1;
    }
  };

  template<typename T>
  std::size_t
  getsize(T t) {
    return getsize_<T>::getsize(t);
  }

  template<typename Cont>
  char const*
  gettype(Cont const &ct) {
    std::size_t const n = getsize(container_max(ct));
    switch(n) {
    case 1:
      return "std::uint8_t";
    case 2:
      return "std::uint16_t";
    default:
      return "std::uint32_t";
    }
  }

  // inspired by makeunicodedata.py splitbins (see python source code)
  template<typename Int>
  void
  splitbins(std::vector<Int> const &t, std::vector<std::size_t> &t1, std::vector<Int> &t2, std::size_t &shift) {
    struct hash {
      std::size_t operator()(std::vector<Int> const &t) const {
        return boost::hash_range(t.begin(), t.end());
      }
    };

    std::size_t const maxshift = floor_log2(t.size());
    std::size_t bytes = ~0;

    for(std::size_t shift2 = 0; shift2 < maxshift + 1; ++shift2) {
      std::size_t const size = 1 << shift2;
      typedef std::unordered_set<std::vector<Int>, hash> bincache_t;
      bincache_t bincache;
      std::size_t t1_size = 0;
      std::size_t t1_max = 0;
      std::size_t t2_size = 0;
      Int t2_max = 0;
      for(std::size_t i = 0; i < t.size() - size; i += size) {
        std::vector<Int> v(t.begin() + i, t.begin() + i + size);
        std::pair<typename bincache_t::iterator, bool> const r = bincache.insert(v);
        if(r.second) {
          t2_size += v.end() - v.begin();
          t2_max = std::max(t2_max, container_max(v));
        }
        t1_max = std::max(t1_max, t2_size >> shift2);
        ++t1_size;
      }
      std::size_t const b = t1_size * getsize(t1_max) + t2_size * getsize(t2_max);
      if(b < bytes) {
        bytes = b;
        shift = shift2;
      }
    }

    std::size_t const size = 1 << shift;
    std::unordered_map<std::vector<Int>, std::size_t, hash> bincache;
    t1.clear();
    t2.clear();
    for(std::size_t i = 0; i < t.size(); i += size) {
      std::vector<Int> v(t.begin() + i, t.begin() + i + size);
      auto iter = bincache.find(v);
      std::size_t index;
      if(iter == bincache.end()) {
        index = t2.size();
        bincache[v] = index;
        t2.insert(t2.end(), v.begin(), v.end());
      }
      else {
        index = iter->second;
      } 
      t1.push_back(index >> shift);
    }  
  }

  template<typename Cont>
  void
  print_list(std::ostream &out, Cont const &ct) {
    std::stringstream linebuf;
    for(auto i = ct.begin(); i != ct.end(); ++i) {
      linebuf << (std::size_t)*i << ", ";
      if(linebuf.str().size() >= 65) {
        out << linebuf.rdbuf() << '\n';
        linebuf.str("");
      }
    }
    out << linebuf.str().substr(0, linebuf.str().size() - 2) << '\n';
  }

  enum Quick_Check {
    Yes,
    Maybe,
    No
  };

  void
  print_case_mapping(std::ostream &out, std::vector<codepoint_t> const &case_map, char const *name) {
    std::size_t shift;
    std::vector<std::size_t> t1;
    std::vector<codepoint_t> casedata;
    splitbins(case_map, t1, casedata, shift);

    out << "std::size_t const " << name << "_shift = " << shift << ";\n\n";

    out << gettype(t1) << " const " << name << "_index[] = {\n";
    print_list(out, t1);
    out << "};\n\n";

    out << "libuni::codepoint_t const " << name << "[] = {\n";
    print_list(out, casedata);
    out << "};\n\n";
  }

  enum break_value_shift {
    Word = 0,
    Sentence = 4,
    Grapheme = 8
  };

  /**
   * Checks whether the string codepoint contains a single code point or a code point range and
   * uses binary-OR to append value to each code point entry in  the random-access container ct.
   */
  template<typename Cont, typename T>
  void
  assign_codepoint(std::string const &codepoint, Cont &ct, T value) {
    std::string::size_type const dot = codepoint.find('.');
    if(dot != std::string::npos) { // range
      codepoint_t i = string_to_codepoint(codepoint.begin(), codepoint.begin() + dot);
      if(dot+2 > codepoint.size()) { // bad code range format!
        std::cerr << "WARNING: Bad Code Range Format `" << codepoint << "'\n";
        throw "todo";
      }
      codepoint_t const end = string_to_codepoint(codepoint.begin() + dot, codepoint.end());
      for(; i <= end; ++i) {
        ct[i] |= value;
      }
    }
    else {
      codepoint_t cp = string_to_codepoint(codepoint.begin(), codepoint.end());
      ct[cp] |= value;
    }
  }

  bool
  text_segmentation() {
    std::vector<std::uint16_t> break_values(0xff0000, 0);
    std::vector<std::string> value_names(1, "X_none");

    std::ifstream in(UCD_PATH "auxiliary/WordBreakProperty" UCD_VERSION ".txt");
    if(not in) {
      std::cerr << "Failed to open: `" UCD_PATH "auxiliary/SentenceBreakProperty" UCD_VERSION ".txt'\n";
      return false;
    }
    for(boost::optional<std::vector<std::string>> line; in; line = parse_line(in)) {
      if(not line or line->size() < 2) {
        continue;
      }
      std::size_t value = insert_unique(value_names, (*line)[1]);
      assign_codepoint((*line)[0], break_values, value);
    }
    in.close();

    // TODO Sentence/GraphemeCluster ...

    std::ofstream out(OUTDIR "segmentation_database.hpp");
    if(not out) {
      std::cerr << "Failed to open: `" OUTDIR "segmentation_database.hpp'\n";
      return false;
    }
    out <<
      "#ifndef LIBUNI_GENERATED_SEGMENTATION_DATABASE_HPP\n"
      "#define LIBUNI_GENERATED_SEGMENTATION_DATABASE_HPP\n\n"
      "//This file is autogenerated by create_two_stage_table.c++\n\n"
      "#include <libuni/codepoint.hpp>\n"
      "#include <cstddef>\n"
      "#include <cstdint>\n\n"
      "namespace {\n";

    out << "enum value_names {\n";
    for(std::size_t i = 0; i < value_names.size()-1; ++i) {
      out << "  " << value_names[i] << "_ = " << i << ",\n";
    }
    out << "  " << value_names[value_names.size()-1] << "_ = " << value_names.size()-1 << '\n';
    out << "};\n\n";

    std::vector<std::size_t> index;
    std::vector<std::uint16_t> values;
    std::size_t shift;
    splitbins(break_values, index, values, shift);

    out << "std::size_t const break_shift = " << shift << ";\n\n";

    out << gettype(index) << " const breaks_index[] = {\n";
    print_list(out, index);
    out << "};\n\n";

    out << "std::uint16_t const breaks[] = {\n";
    print_list(out, values);
    out << "};\n\n";

    out << "} // namespace\n\n#endif\n";
    return true;
  }
}

#ifndef TEST // This is required by test/test_generate_two_stage_table.c++!
int
main() {
  std::vector<std::uint16_t> qc(0xff0000, 0x0000);

  std::vector<codepoint_t> simple_uppercase_mapping(0xff0000, 0);
  std::vector<codepoint_t> simple_lowercase_mapping(0xff0000, 0);
  std::vector<codepoint_t> simple_titlecase_mapping(0xff0000, 0);

  std::vector<codepoint_t> decomp_map(1, 0); // decomp_map[0] => 0
  std::vector<std::size_t> decomp_index(0xff0000, 0x00);
  std::vector<std::string> decomp_prefix(1, "x_none"); // decomp_prefix[0] => no prefix

  std::ifstream inud(UCD_PATH "UnicodeData" UCD_VERSION ".txt");
  if(not inud) {
    std::cerr << "Failed to open: `" UCD_PATH "UnicodeData" UCD_VERSION ".txt'\n";
    return 1;
  }
  for(boost::optional<std::vector<std::string>> line; inud; line = parse_line(inud)) {
    if(not line or line->size() < 14) {
      continue;
    }

    // 0 . Codepoint
    codepoint_t const cp = string_to_codepoint((*line)[0]);

    // 3. Canonical Combining Class
    std::uint8_t const combining_class = std::stoul((*line)[3]);
    qc[cp] |= std::uint16_t(combining_class) << 8;

    // 5. Decomposition_Type and Decomposition_Mapping
    handle_decomp_mapping(cp, (*line)[5], decomp_index, decomp_map, decomp_prefix);

    // 12. Simple Uppercase Mapping
    simple_uppercase_mapping[cp] = string_to_codepoint((*line)[12]);

    // 13. Simple Lowercase Mapping
    simple_lowercase_mapping[cp] = string_to_codepoint((*line)[13]);

    // 14. Simple Titlecase Mapping
    if(line->size() > 14) {
      simple_titlecase_mapping[cp] = string_to_codepoint((*line)[14]);
    }
  }

  std::ifstream in(UCD_PATH "DerivedNormalizationProps" UCD_VERSION ".txt");
  if(not in) {
    std::cerr << "Failed to open: `" UCD_PATH "DerivedNormalizationProps" UCD_VERSION ".txt'\n";
    return 1;
  }
  for(boost::optional<std::vector<std::string>> line; in; line = parse_line(in)) {
    if(not line) {
      continue;
    }
    else if(line->size() == 3) {
      if((*line)[2].empty()) {
        continue;
      }
      std::uint8_t value = Yes;
      switch((*line)[2][0]) {
      case 'N':
        value = No;
        break;
      case 'M':
        break;
        value = Maybe;
      }

      std::string const &type = (*line)[1];
      unsigned shift;
      if(type == "NFD_QC") {
        shift = 0;
      }
      else if(type == "NFKD_QC") {
        shift = 2;
      }
      else if(type == "NFC_QC") {
        shift = 4;
      }
      else if(type == "NFKC_QC") {
        shift = 6;
      }

      assign_codepoint((*line)[0], qc, value << shift);
    }
  }

  std::ofstream out(OUTDIR "normalization_database.hpp");
  if(not out) {
    std::cerr << "Failed to open: `" OUTDIR "normalization_database.hpp'\n";
    return 1;
  }
  out <<
    "#ifndef LIBUNI_GENERATED_NORMALIZATION_DATABASE_HPP\n"
    "#define LIBUNI_GENERATED_NORMALIZATION_DATABASE_HPP\n\n"
    "//This file is autogenerated by create_two_stage_table.c++\n\n"
    "#include <libuni/codepoint.hpp>\n"
    "#include <cstdint>\n\n"
    "namespace {\n";

  // Quick Check
  std::vector<std::size_t> t1;
  std::vector<std::uint16_t> t2;
  std::size_t shift;
  splitbins(qc, t1, t2, shift);

  out << "std::size_t const quick_check_shift = " << shift << ";\n\n";

  out << gettype(t1) << " const quick_check_index[] = {\n";
  print_list(out, t1);
  out << "};\n\n"; 

  out << "std::uint16_t const quick_check[] = {\n";
  print_list(out, t2);
  out << "};\n\n";

  // Decomp
  out << "libuni::codepoint_t const decomp_map[] = {\n";
  print_list(out, decomp_map);
  out << "};\n\n";

  qc.clear(); // free memory
  t2.clear();
  std::vector<std::size_t> decomp_index2;
  splitbins(decomp_index, t1, decomp_index2, shift);

  out << "std::size_t const decomp_shift = " << shift << ";\n\n";

  out << gettype(t1) << " const decomp_index1[] = {\n";
  print_list(out, t1);
  out << "};\n\n";
  t1.clear();

  out << gettype(decomp_index2) << " const decomp_index2[] = {\n";
  print_list(out, decomp_index2);
  out << "};\n\n";
  decomp_index2.clear();

  out << "} // namespace\n\n#endif\n";
  out.close();

  qc.clear();
  decomp_index.clear();

  // Case Database
  out.open(OUTDIR "case_database.hpp");
  if(not out) {
    std::cerr << "Failed to open: `" OUTDIR "case_database.hpp'\n";
    return 1;
  }

  out <<
    "#ifndef LIBUNI_GENERATED_CASE_DATABASE_HPP\n"
    "#define LIBUNI_GENERATED_CASE_DATABASE_HPP\n\n"
    "//This file is autogenerated by create_two_stage_table.c++\n\n"
    "#include <libuni/codepoint.hpp>\n"
    "#include <cstdint>\n\n"
    "namespace {\n";

  print_case_mapping(out, simple_uppercase_mapping, "simple_uppercase_mapping");
  simple_uppercase_mapping.clear();
  print_case_mapping(out, simple_lowercase_mapping, "simple_lowercase_mapping");
  simple_lowercase_mapping.clear();
  print_case_mapping(out, simple_titlecase_mapping, "simple_titlecase_mapping");
  simple_titlecase_mapping.clear();

  out << "} // namespace\n\n#endif\n";

  // Text Segmentation (GraphemeBreak, LineBreak, SentenceBreak, WordBreak)
  if(not text_segmentation()) {
    return 1;
  }
}

#endif //TEST
