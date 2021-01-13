#pragma once

#include "peutil-config.h"

#include <string>
#include <unordered_map>
#include <deque>

using namespace std;

class PathCache {
  private:
    unordered_map<string, unordered_map<string, string> > m_;
    string resolve(const unordered_map<string, string> &h, const string &filename);
    #ifdef PEUTIL_FORCE_FS_WINDOWS_SEPARATOR
      static constexpr char _sep = '/';
    #else
      static constexpr char _sep = (char)fs::path::preferred_separator;
    #endif
  public:
    string resolve(const deque<string> &search_path, const string &filename);
};