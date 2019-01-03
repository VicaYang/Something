#pragma once
#include <QStandardItemModel>
#include <QMutex>
#include "USNParser.h"
#include "FileIndex.h"
#include "Monitor.h"

enum class UpdateType { ADD, REMOVE, CONTENT_CHANGE };
class Searcher {
public:
  Searcher(std::vector<char>& _drivers);
  void parseQuery(std::wstring& query);
  void searchPath(std::wstring& path);
  void searchContent(std::wstring& content);
  void filter(std::wstring& newpath);
  bool recvPUSN(int id, PUSN_RECORD pusn);
  bool update(FileEntry* entry, UpdateType type);
  std::wstring addHighLight(std::wstring& path);
  std::vector<std::wstring> recommend() const;
  std::vector<Monitor*> monitors;
  std::vector<USNParser*> drivers;
  std::vector<FileIndex*> indexs;
  std::set<FileEntry*> content_result;
  std::set<FileEntry*> path_result;
  std::vector<std::wstring> splited;
  std::wstring _content;
  std::wstring _path;
  bool singleSearch(std::wstring& path, std::wstring& query_path);
  QMutex lock;
};