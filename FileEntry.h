#pragma once
#include <windows.h>
#include <string>
#include <map>
#include <vector>
#define ROOT_REFERENCE 0x5000000000005
typedef unsigned long long FILEREF;

class FileEntry {
public:
  FileEntry(const PUSN_RECORD usnRecord, const char driver_letter);
  FileEntry(const char driver_letter);
  FileEntry(const FileEntry&) = delete;
  FileEntry& operator=(const FileEntry&) = delete;
  void genPath(std::map<FILEREF, FileEntry*>& table);
  ~FileEntry();
  FILEREF file_ref;
  FILEREF parent_ref;
  std::wstring file_name;
  std::wstring full_path;
  bool is_folder;
  char driver_letter;
  bool operator<(const FileEntry& rhs);
  bool operator==(const FileEntry& rhs);
};

