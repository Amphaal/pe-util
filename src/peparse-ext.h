#pragma once

#include <parser-library/parse.h>

#include <string>
#include <vector>

using namespace std;

namespace peparse {

// XXX make static in library ...
extern ::uint32_t err;
extern string err_loc;

// XXX duplicated from parse.cc
struct section {
  string                sectionName;
  ::uint64_t            sectionBase;
  bounded_buffer        *sectionData;
  image_section_header  sec;
};

struct parsed_pe_internal {
  vector<section> secs;
};

#define READ_DWORD_NULL(b, o, inst, member)                                 \
  if (!readDword(b, o + _offset(__typeof__(inst), member), inst.member)) {  \
    PE_ERR(PEERR_READ);                                                     \
    return nullptr;                                                         \
  }

// XXX library symbols are too generic
extern bool getHeader(bounded_buffer *file, pe_header &p, bounded_buffer *&rem);
extern bool getSections( bounded_buffer  *b,
                        bounded_buffer  *fileBegin,
                        nt_header_32    &nthdr,
                        vector<section>   &secs);
extern bool getSecForVA(const vector<section> &secs, VA v, section &sec);

}