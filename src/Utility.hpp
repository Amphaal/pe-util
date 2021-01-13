#pragma once

#include <string>
#include <algorithm>

using namespace std;

static string _to_lower(const string &str) {
  // Not UTF-8 safe, use ICU?
  string result(str);
  transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}