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
#include <FileEntry.h>
#include "USNParser.h"

class FileInfo {
public:
	FILEREF FileNum;
	std::wstring FileName;
	std::wstring FilePath;
	std::wstring FileContent;
	FileInfo(FILEREF num, const std::wstring& path, QProcess *process);
	std::wstring identifyName(const std::wstring& path);
	std::vector<std::wstring> words;
};

class post {
public:
	FILEREF FileNum;
	int FreqNum;
};

class FileIndex {
public:
	std::list<FileInfo> Files;
	std::unordered_map<std::wstring, std::list<post>> DB;
	QProcess *process;
	FileIndex(USNParser* driver, QProcess* mprocess);
	void InsertFiles(const std::wstring& dir);
	void InsertFile(FILEREF num, const std::wstring& path);
	void DeleteFile(const std::wstring& path);
	std::set<FileEntry*> SearchFile(const std::wstring& sentence);
	USNParser* driver;
};
