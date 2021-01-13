#pragma once

#include <deque>
#include <vector>
#include <unordered_set>
#include <string>
#include <ostream>
#include <iostream>

using namespace std;

struct Arguments {
  bool resolve {false};
  bool transitive {false};
  bool include_main {false};
  deque<string> files;
  deque<string> search_path;
  bool no_default_search_path {false};
  const vector<string> mingw64_search_path = {
    "/usr/x86_64-w64-mingw32/sys-root/mingw/bin"
  };
  const vector<string> mingw64_32_search_path = {
    "/usr/i686-w64-mingw32/sys-root/mingw/bin"
  };
  unordered_set<string> whitelist;
  const vector<string> default_whitelist = {
    // lower-case because windows is case insensitive ...
    "advapi32.dll",
    "/api-ms-.*\\.dll/",
    "avicap32.dll",
    "bcrypt.dll",
    "comctl32.dll",
    "comdlg32.dll",
    "credui.dll",
    "crypt32.dll",
    "cryptui.dll",
    "d3d11.dll",
    "d3d9.dll",
    "dbghelp.dll",
    "dhcpcsvc.dll",
    "dnsapi.dll",
    "dwmapi.dll",
    "dwrite.dll",
    "dxgi.dll",
    "dxva2.dll",
    "gdi32.dll",
    "hid.dll",
    "imm32.dll",
    "iphlpapi.dll",
    "kernel32.dll",
    "mpr.dll",
    "msimg32.dll",
    "msvcrt.dll",
    "ncrypt.dll",
    "netapi32.dll",
    "normaliz.dll",
    "ole32.dll",
    "oleacc.dll",
    "oleaut32.dll",
    "powrprof.dll",
    "propsys.dll",
    "psapi.dll",
    "secur32.dll",
    "setupapi.dll",
    "shell32.dll",
    "shlwapi.dll",
    "urlmon.dll",
    "user32.dll",
    "userenv.dll",
    "userenv.dll",
    "usp10.dll",
    "uxtheme.dll",
    "version.dll",
    "winhttp.dll",
    "wininet.dll",
    "winmm.dll",
    "winspool.dll",
    "winspool.drv",
    "wldap32.dll",
    "ws2_32.dll",
    "wtsapi32.dll",
    "rpcrt4.dll",
    "opengl32.dll"
  };
  bool no_default_whitelist {false};
  bool ignore_errors {false};

  void parse(int argc, char **argv);
  void help(ostream &o, const char *argv0);
 
 private:
  static string _get_cwd();
  static vector<string> _get_path_dirs();
};
