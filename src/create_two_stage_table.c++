#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <cstdint>
#include <unordered_map>

#include <boost/optional.hpp>
#include <boost/functional/hash.hpp>

namespace libuni {
  typedef std::uint32_t codepoint_t;

  namespace {
    template<typename I>
    codepoint_t string_to_codepoint(I begin, I end) {
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

    // Data files are in UTF-8 but non-ASCII characters only in comments (UAX#44: 4.29)
    boost::optional<std::vector<std::string>> parse_line(std::istream &in) {
      std::string line;
      if(!std::getline(in, line) or line.empty() or line[0] == '#') {
        return boost::none;
      }
      std::string::const_iterator i = line.begin();
      std::string::const_iterator const end = line.end();
      std::vector<std::string> properties;
      for(std::string::const_iterator from = i;; ++i) {
        if(*i == ';') {
          properties.push_back(create_trimmed_string(from, i));
          from = i;
          ++from;
        }
        else if(i == end and from != end) {
          properties.push_back(create_trimmed_string(from, end));
          break;
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

  template<typename Cont>
  std::size_t getsize(Cont const &ct) {
    typename Cont::value_type const max = container_max(ct);
    if(max <= 0xFF)
      return 1;
    else if(max <= 0xFFFF)
      return 2;
    else
      return 4;
  }

  template<typename Cont>
  char const *gettype(Cont const &ct) {
    std::size_t const n = getsize(ct);
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
  void splitbins(std::vector<Int> const &t, std::vector<std::size_t> &t1, std::vector<Int> &t2, std::size_t &shift) {
    struct hash {
      std::size_t operator()(std::vector<Int> const &t) const {
        return boost::hash_range(t.begin(), t.end());
      }
    };

    std::size_t const maxshift = floor_log2(t.size());
    std::size_t bytes = ~0;
    std::size_t best = 0;

    for(std::size_t shift2 = 0; shift2 < maxshift + 1; ++shift2) {
      std::size_t const size = 1 << shift2;
      typedef std::unordered_map<std::vector<Int>, std::size_t, hash> bincache_t;
      bincache_t bincache;
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
        t1.push_back(index >> shift2);
      }
      std::size_t b = t1.size() * getsize(t1) + t2.size()*getsize(t2);
      if(b < bytes) {
        bytes = b;
        best = shift2;
      }
    }

    shift = best;
    std::size_t const size = 1 << best;
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
      t1.push_back(index >> best);
    }  
  }

  template<typename Cont>
  void print_list(std::ostream &out, Cont const &ct) {
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
}
using namespace libuni;

#ifndef UNIDATA
#define UNIDATA "UCD/"
#endif
#ifndef UNIDATA_VERSION
#define UNIDATA_VERSION
#endif

//#define TEST

#ifndef TEST

int main() {
  std::ifstream in(UNIDATA "DerivedNormalizationProps" UNIDATA_VERSION ".txt");
  if(!in) {
    std::cerr << "Failed to open: `" << UNIDATA "DerivedNormalizationProps" UNIDATA_VERSION ".txt" << "'\n";
    return 1;
  }
  std::vector<std::uint8_t> qc(0xff0000, 0x00);
  for(boost::optional<std::vector<std::string>> line; in; line = parse_line(in)) {
    if(not line) {
      continue;
    }
    else if(line->size() == 2) {
    }
    else if(line->size() == 3) {
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

      std::string const &codepoint = (*line)[0];
      std::string::size_type const dot = codepoint.find('.');
      if(dot != std::string::npos) { // range
        codepoint_t i = string_to_codepoint(codepoint.begin(), codepoint.begin() + dot);
        if(dot+2 > codepoint.size()) { // bad code range format!
          std::cerr << "WARNING: Bad Code Range Format `" << codepoint << "'\n";
          break;
        }
        codepoint_t const end = string_to_codepoint(codepoint.begin() + dot, codepoint.end());
        for(; i != end; ++i) {
          qc[i] |= value << shift;
        }
      }
      else {
        codepoint_t cp = string_to_codepoint(codepoint.begin(), codepoint.end());
        qc[cp] |= value << shift;
      }
    }
  }

  std::vector<std::size_t> t1;
  std::vector<std::uint8_t> t2;
  std::size_t shift;
  splitbins(qc, t1, t2, shift);

  std::ofstream out("normalization_database.hpp");
  out <<
    "#ifndef LIBUNI_GENERATED_NORMALIZATION_DATABASE_HPP\n"
    "#define LIBUNI_GENERATED_NORMALIZATION_DATABASE_HPP\n\n"
    "//This file is autogenerated by create_two_stage_table.c++\n\n"
    "namespace libuni {\n";

  out << "std::size_t const quick_check_shift = " << shift << ";\n\n";

  out << gettype(t1) << " quick_check_index[] = {\n";
  print_list(out, t1);
  out << "};\n\n"; 

  out << "std::uint8_t quick_check[] = {\n";
  print_list(out, t2);
  out << "};\n\n";

  out << "} // namespace libuni\n";
}

#else

int main() { // DBG
  // string_to_codepoint
  {
    std::string a("AFFE");
    codepoint_t t = string_to_codepoint(a.begin(), a.end());
    assert(t == 0xAFFE);
    a = "3a";
    t = string_to_codepoint(a.begin(), a.end());
    assert(t == 0x3A);
  }

  // create_trimmed_string
  {
    std::string a(" ab ");
    std::string b(create_trimmed_string(a.begin(), a.end()));
    assert(b == "ab");
  }

  // parse_line
  {
    std::stringstream ss;
    ss << "a;b;c # hello\nd;e;f\n";
    boost::optional<std::vector<std::string>> l = parse_line(ss);
    assert(l);
    assert(l->size() == 3);
    assert((*l)[0] == "a");
    assert((*l)[1] == "b");
    assert((*l)[2] == "c");
    boost::optional<std::vector<std::string>> k = parse_line(ss);
    assert(k);
    assert(k->size() == 3);
    assert((*k)[0] == "d");
    assert((*k)[1] == "e");
    assert((*k)[2] == "f");
  }
}
#endif
