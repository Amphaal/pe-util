#include "PathCache.h"
#include "Utility.hpp"

#include <filesystem>

namespace fs = std::filesystem;

#ifdef PEUTIL_FORCE_FS_WINDOWS_SEPARATOR
  static constexpr char _sep = '/';
#else
  static constexpr char _sep = (char)std::filesystem::path::preferred_separator;
#endif

string PathCache::resolve(const unordered_map<string, string> &h, const string &filename) {
  auto fn = _to_lower(filename);
  auto i = h.find(fn);
  if (i == h.end())
    throw range_error("Could not resolve: " + filename);
  return i->second;
}

string PathCache::resolve(const deque<string> &search_path, const string &filename) {
  for (auto path : search_path) {
    if (fs::exists(path)) {
      try {
        auto i = m_.find(path);
        if (i == m_.end()) {
          unordered_map<string, string> xs;
          for (auto &e : fs::directory_iterator(path)) {
            auto fn = e.path().filename().string();
            xs[_to_lower(fn)] = std::move(fn);
          }
          auto r = m_.insert(make_pair(path, std::move(xs)));
          return path + _sep + resolve(r.first->second, filename);
        } else {
          return path + _sep + resolve(i->second, filename);
        }
      }
      catch (const range_error &e) {
        continue;
      }
    }
    else
      continue;
  }
  throw range_error("Could not resolve: " + filename);
}
