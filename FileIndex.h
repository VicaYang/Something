#pragma once
#pragma comment(lib, "NLPIR.lib")
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include "Reader.h"
#include"NLPIR.h"


class FileInfo {
public:
	int FileNum;
	std::wstring FileName;
	std::wstring FilePath;
	std::wstring FileContent;
	FileInfo(int num, const std::wstring& path);
	std::wstring identifyName(const std::wstring& path);
	std::vector<std::wstring> words;
};

class post {
public:
	int FileNum;
	int FreqNum;
};

class FileIndex {
public:
	int FileNum;
	std::vector<FileInfo> Files;
	std::unordered_map<std::wstring, std::vector<post>> DB;
	FileIndex();
	void InsertFile(const std::wstring& path);
	std::vector<FileInfo> SearchFile(const std::wstring& sentence);
};
