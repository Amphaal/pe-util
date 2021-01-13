#include "Arguments.h"
#include "Utility.hpp"

#include <cstring>
#include <filesystem>
#include <sstream>

void Arguments::parse(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    auto a = argv[i];
    if (!strcmp(a, "-a") || !strcmp(a, "--all")) {
      resolve = true;
      transitive = true;
      include_main = true;
    } else if (!strcmp(a, "-t") || !strcmp(a, "--transitive")) {
      transitive = true;
      resolve = true;
    } else if (!strcmp(a, "-r") || !strcmp(a, "--resolve")) {
      resolve = true;
    } else if (!strcmp(a, "-p") || !strcmp(a, "--path")) {
      if (++i >= argc)
        throw runtime_error("path argument is missing");
      search_path.push_back(argv[i]);
    } else if (!strcmp(a, "--no-path") || !strcmp(a, "--clear-path")) {
      no_default_search_path = true;
    } else if (!strcmp(a, "--search-env")) {
      for (auto &path : _get_path_dirs())
        search_path.push_back(path);
    } else if (!strcmp(a, "--search-cwd")) {
      search_path.push_back(_get_cwd());
    } else if (!strcmp(a, "-w") || !strcmp(a, "--wlist")) {
      if (++i >= argc)
        throw runtime_error("whitelist argument is missing");
      whitelist.insert(_to_lower(string(argv[i])));
    } else if (!strcmp(a, "--no-wlist") || !strcmp(a, "--clear-wlist")) {
      no_default_whitelist = true;
    } else if (!strcmp(a, "--ignore-errors")) {
      ignore_errors = true;
    } else if (!strcmp(a, "-h") || !strcmp(a, "--help")) {
      help(cout, *argv);
      exit(0);
    } else if (!strcmp(a, "--")) {
      for (int k = ++i; k < argc; ++k)
        files.push_back(argv[k]);
    } else if (*a == '-') {
      throw runtime_error("Unknown option: " + string(a));
    } else {
      files.push_back(a);
    }
  }

  if (!no_default_whitelist)
    whitelist.insert(default_whitelist.begin(), default_whitelist.end());

  #ifdef PEUTIL_WHITELIST_EXTENSIONS 
    auto wl_exts = _split(PEUTIL_WHITELIST_EXTENSIONS_, ';');
    whitelist.insert(wl_exts.begin(), wl_exts.end());
  #endif
}

string Arguments::_get_cwd() {
  return std::filesystem::current_path().string();
}

vector<string> Arguments::_split(const string &s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string item;
    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

vector<string> Arguments::_get_path_dirs() {
  vector<string> result;
#if WIN32
  char delim = ';';
#else
  char delim = ':';
#endif
  string PATH = getenv("PATH");
  for (size_t start, end = 0; (start = PATH.find_first_not_of(delim, end)) != string::npos; ) {
    end = PATH.find(delim, start);
    result.push_back(PATH.substr(start, end != string::npos ? end - start : end));
  }
  return result;
}

void Arguments::help(ostream &o, const char *argv0) {
  o << "call: " << argv0 << " (OPTION)* foo.exe\n"
    << "  or: " << argv0 << " (OPTION)* foo.dll\n"
     "\n\n\nwhere OPTION  is one of:\n"
     "  -h, --help           this screen\n"
     "  -r, --resolve        resolve a dependency using a search path\n"
     "  -t, --transitive     transitively list the dependencies, implies -r\n"
     "  -a, --all            imply -t,-r and include the input PEs\n"
     "  -p, --path           build custom search path\n"
     "      --no-path\n"
     "      --clear-path     don't include the default mingw64/-32 path\n"
     "      --search-env     add all PATH directories to search path\n"
     "      --search-cwd     add current working directory to search path\n"
     "  -w  --wlist          whitelist a library name\n"
     "      --no-wlist\n"
     "      --clear-wlist    don't populate the whitelist with defaults\n"
     "      --ignore-errors  ignore library-not-found errors\n"
       "\n"
       ;
}