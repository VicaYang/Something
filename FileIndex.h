#pragma once
#pragma comment(lib, "NLPIR.lib")
#include <string>
#include <math.h>
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
#include <algorithm>
#include "USNParser.h"


class fword {
public:
	std::wstring word;
	double tfidf;
	int freq;
};

class FileInfo {
public:
	FILEREF FileNum;
	std::wstring FileName;
	std::wstring FilePath;
	std::wstring FileContent;
	int max_freq;
	FileInfo(FILEREF num, const std::wstring& path);
	std::wstring identifyName(const std::wstring& path);
	std::vector<fword> words;
};



class post {
public:
	FILEREF FileNum;
	double FreqNum;
};

class FileIndex {
public:
	std::list<FileInfo> Files;
	std::unordered_map<std::wstring, std::list<post>> DB;
	FileIndex(USNParser* driver);
	~FileIndex();
	void InsertFiles(const std::wstring& dir);
	void InsertFileIndex(FILEREF num, const std::wstring& path);
	void DeleteFileIndex(const std::wstring& path);
	bool exist(FILEREF num);
	std::set<FileEntry*> SearchFile(const std::wstring& sentence);
	void Calctfidf();
	USNParser* driver;
};
