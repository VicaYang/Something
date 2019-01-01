#pragma once
#pragma comment(lib, "NLPIR.lib")
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <list>
#include "Reader.h"
#include "NLPIR.h"
#include <qprocess.h>

class FileInfo {
public:
	int FileNum;
	std::wstring FileName;
	std::wstring FilePath;
	std::wstring FileContent;
	FileInfo(int num, const std::wstring& path, QProcess *process);
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
	std::list<FileInfo> Files;
	std::unordered_map<std::wstring, std::list<post>> DB;
	QProcess *process;
	FileIndex(QProcess *mprocess);
  void InsertFiles(const std::wstring& dir);
	void InsertFile(const std::wstring& path);
	void DeleteFile(const std::wstring& path);
	std::vector<FileInfo> SearchFile(const std::wstring& sentence);
};
