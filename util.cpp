#include "util.h"
std::vector<std::wstring> strsplit(const std::wstring& str, wchar_t sep) {
 std::vector<std::wstring> res;
 auto loc = 0;
 while (loc < str.length()) {
   while (loc < str.length() && str[loc] == sep) loc++;
   if (loc >= str.length()) break;
   auto nextLoc = loc + 1;
   while (nextLoc < str.length() && str[nextLoc] != sep) nextLoc++;
   res.push_back(str.substr(loc, nextLoc - loc));
   loc = nextLoc + 1;
 }
 return res;
}