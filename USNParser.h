#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include "FileEntry.h"
class USNParser
{
public:
    USNParser(const char dl);
    ~USNParser();
    bool createUsnJournal();
    void genEntries();
    void getRootHandle();

    std::vector<FileEntry*> query(const std::wstring& pattern);
    void recursiveAdd(FILEREF folder, std::vector<FileEntry*>& res);

    char driver_letter;
    HANDLE root_handle;
    std::map<FILEREF, FileEntry*> all_entries;
    std::map<FILEREF, std::vector<FileEntry*>> sub_entries;
};