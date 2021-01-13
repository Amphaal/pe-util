#pragma once

#include <unordered_map>
#include <stack>
#include <string>
#include <set>

#include "Arguments.h"

using namespace std;

class Traverser {
  private:
    const Arguments &args;
    unordered_map<string, string> known_files; // basename, name
    stack<pair<string, string> > files;
    set<string> result_set;
    bool _regex_match(const string &r, const string &s);
    bool _wlist_match(const unordered_set<string> &wlist, const string &s);
  public:
    Traverser(const Arguments &args);
    void prepare_stack();
    void process_stack();
    void print_result();
};
