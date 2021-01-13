#include "Traverser.h"
#include "PathCache.h"
#include "Utility.hpp"
#include "peldd.hpp"

#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

Traverser::Traverser(const Arguments &args) : args(args) {
  prepare_stack();
  process_stack();
  print_result();
}

void Traverser::prepare_stack() {
  for (auto &a : args.files) {
    auto p = make_pair(fs::path(a).filename().string(), a);
    if (!known_files.count(p.first)) {
      if (args.include_main)
        result_set.insert(p.second);
      known_files.insert(p);
      files.push(std::move(p));
    }
  }
}

void Traverser::process_stack() {
  PathCache path_cache;
  while (!files.empty()) {
    auto t = files.top();
    files.pop();
    deque<string> search_path(args.search_path);
    auto p = names(t.second.c_str());
    auto &ns = p.first;
    auto is64 = p.second;
    if (!args.no_default_search_path) {
      if (is64)
        search_path.insert(search_path.begin(),
            args.mingw64_search_path.begin(), args.mingw64_search_path.end());
      else
        search_path.insert(search_path.begin(),
            args.mingw64_32_search_path.begin(),
            args.mingw64_32_search_path.end());
    }
    for (auto &n : ns) {
      if (_wlist_match(args.whitelist, n))
        continue;
      if (args.resolve) {
        try {
          auto resolved = path_cache.resolve(search_path, n);
          result_set.insert(resolved);
          if (args.transitive && !known_files.count(n)) {
            auto p = make_pair(n, resolved);
            known_files.insert(p);
            files.push(std::move(p));
          }
        } catch (const range_error &e) {
          if (args.ignore_errors) {
            cerr << e.what() << '\n';
            continue;
          }
          throw;
        }
      } else {
        result_set.insert(n);
      }
    }
  }
}

bool Traverser::_regex_match(const string &r, const string &s) {
  return regex_match(s, regex(r, regex_constants::ECMAScript | regex_constants::icase));
}

bool Traverser::_wlist_match(const unordered_set<string> &wlist, const string &s) {
  for (auto &w : wlist) {
    // match regex if in /regex/ format
    if (w.length() > 1 && w.at(0) == '/' && w.at(w.length() - 1) == '/') {
      if (_regex_match(w.substr(1, w.length() - 2), s))
        return true;
    }
    // match literal
    else if (w == _to_lower(s))
      return true;
  }
  return false;
}

void Traverser::print_result() {
  for (auto &r : result_set)
    cout << r << '\n';
}
