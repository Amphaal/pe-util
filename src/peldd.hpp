//
// That code is licensed as:
/*
The MIT License (MIT)

Copyright (c) 2013 Andrew Ruef

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "peparse-ext.h"

#include <deque>
#include <filesystem>
#include <cstring>

using namespace peparse;

// most of the following function body is copied from ParsePEFromFile()
// (cf. pe-parse/pe-parser-library/src/parse.cpp)
static parsed_pe *names_prime(const char *filePath, deque<string> &ns, bool &is64) {
  //first, create a new parsed_pe structure
  auto *p = new parsed_pe();

  if(p == nullptr) {
    PE_ERR(PEERR_MEM);
    return nullptr;
  }

  //make a new buffer object to hold just our file data
  p->fileBuffer = readFileToFileBuffer(filePath);

  if(p->fileBuffer == nullptr) {
    delete p;
    // err is set by readFileToFileBuffer
    return nullptr;
  }

  p->internal = new parsed_pe_internal();

  //get header information
  bounded_buffer  *remaining = nullptr;
  if(!getHeader(p->fileBuffer, p->peHeader, remaining)) {
    deleteBuffer(p->fileBuffer);
    delete p;
    // err is set by getHeader
    return nullptr;
  }

  bounded_buffer  *file = p->fileBuffer;
  if(!getSections(remaining, file, p->peHeader.nt, p->internal->secs)) {
    deleteBuffer(remaining);
    deleteBuffer(p->fileBuffer);
    delete p;
    PE_ERR(PEERR_SECT);
    return nullptr;
  }

   
  //get imports
  data_directory importDir;
  if (p->peHeader.nt.OptionalMagic == NT_OPTIONAL_32_MAGIC) {
    importDir = p->peHeader.nt.OptionalHeader.DataDirectory[DIR_IMPORT];
    // GS: also return this information
    is64 = false;
  } else if (p->peHeader.nt.OptionalMagic == NT_OPTIONAL_64_MAGIC) {
    importDir = p->peHeader.nt.OptionalHeader64.DataDirectory[DIR_IMPORT];
    is64 = true;
  } else {
    deleteBuffer(remaining);
    deleteBuffer(p->fileBuffer);
    delete p;
    PE_ERR(PEERR_MAGIC);
    return nullptr;
  }

  if(importDir.Size != 0) {
    //get section for the RVA in importDir
    section c;
    VA addr;
    if (p->peHeader.nt.OptionalMagic == NT_OPTIONAL_32_MAGIC) {
      addr = importDir.VirtualAddress + p->peHeader.nt.OptionalHeader.ImageBase;
    } else if (p->peHeader.nt.OptionalMagic == NT_OPTIONAL_64_MAGIC) {
      addr = importDir.VirtualAddress + p->peHeader.nt.OptionalHeader64.ImageBase;
    } else {
      deleteBuffer(remaining);
      deleteBuffer(p->fileBuffer);
      delete p;
      PE_ERR(PEERR_MAGIC);
      return nullptr;
    }

    if(!getSecForVA(p->internal->secs, addr, c)) {
      deleteBuffer(remaining);
      deleteBuffer(p->fileBuffer);
      delete p;
      PE_ERR(PEERR_READ);
      return nullptr;
    }

    //get import directory from this section
    ::uint32_t offt = addr - c.sectionBase;

    import_dir_entry emptyEnt;
    memset(&emptyEnt, 0, sizeof(import_dir_entry));

    do {
      //read each directory entry out
      import_dir_entry curEnt = emptyEnt;

      READ_DWORD_NULL(c.sectionData, offt, curEnt, LookupTableRVA);
      READ_DWORD_NULL(c.sectionData, offt, curEnt, TimeStamp);
      READ_DWORD_NULL(c.sectionData, offt, curEnt, ForwarderChain);
      READ_DWORD_NULL(c.sectionData, offt, curEnt, NameRVA);
      READ_DWORD_NULL(c.sectionData, offt, curEnt, AddressRVA);

      //are all the fields in curEnt null? then we break
      if( curEnt.LookupTableRVA == 0 && 
          curEnt.NameRVA == 0 &&
          curEnt.AddressRVA == 0) {
        break;
      }

      //then, try and get the name of this particular module...
      VA name;
      if (p->peHeader.nt.OptionalMagic == NT_OPTIONAL_32_MAGIC) {
        name = curEnt.NameRVA + p->peHeader.nt.OptionalHeader.ImageBase;
      } else if (p->peHeader.nt.OptionalMagic == NT_OPTIONAL_64_MAGIC) {
        name = curEnt.NameRVA + p->peHeader.nt.OptionalHeader64.ImageBase;
      } else {
        deleteBuffer(remaining);
        deleteBuffer(p->fileBuffer);
        delete p;
        PE_ERR(PEERR_MAGIC);
        return nullptr;
      }

      section nameSec;
      if(!getSecForVA(p->internal->secs, name, nameSec)) {
        PE_ERR(PEERR_SECTVA);
        deleteBuffer(remaining);
        deleteBuffer(p->fileBuffer);
        delete p;
        return nullptr;
      }

      ::uint32_t nameOff = name - nameSec.sectionBase;

      // GS: replace original byte-by-byte copy version
      if (nameOff < nameSec.sectionData->bufLen) {
        auto p = nameSec.sectionData->buf;
        auto n = nameSec.sectionData->bufLen;
        auto b = p + nameOff;
        auto x = std::find(b, p + n, 0);
        ns.emplace_back(b, x);
      }

      offt += sizeof(import_dir_entry);
    } while(true);
  }

  deleteBuffer(remaining);

  return p;
}

static pair<deque<string>, bool> names(const char *filename) {
  if (!std::filesystem::exists(filename))
    throw runtime_error("File doesn't exist: " + string(filename));
  deque<string> ns;
  bool is64 = false;
  auto p = names_prime(filename, ns, is64);
  if (!p)
    throw runtime_error("Error reading PE structure: " + err_loc);
  deleteBuffer(p->fileBuffer);
  for (auto &s : p->internal->secs)
    delete s.sectionData;
  delete p->internal;
  delete p;
  return make_pair(std::move(ns), is64);
}

