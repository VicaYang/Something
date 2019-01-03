#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include "FileEntry.h"

class USNParser
{
public:
  USNParser(const char dl);
  ~USNParser();
  bool createUsnJournal();
  void genEntries();
  void cleanHiddenEntries();
  void getRootHandle();
  void addFileEntry(FileEntry* entry);
  std::set<FileEntry*> query(const std::wstring& pattern);
  std::set<FileEntry*> querySimple(const std::wstring& pattern);
  std::set<FileEntry*> queryComplex(const std::wstring& pattern);
  void recursiveAdd(FILEREF folder, std::set<FileEntry*>& res);
  FILEREF getFileRef(const std::wstring& path);

  char driver_letter;
  HANDLE root_handle;
  USN_JOURNAL_DATA journal;
  std::map<FILEREF, FileEntry*> all_entries;
  std::map<FILEREF, std::vector<FileEntry*>> sub_entries;
  std::map<FILEREF, FileEntry*> recycle;
  const int max_search = 1000;
};