#pragma once
#include <QStandardItemModel>
#include "USNParser.h"
#include "FileIndex.h"

class Searcher {
public:
  Searcher(std::vector<USNParser*> drivers, std::vector<FileIndex*> indexs, QStandardItemModel* model) :
    drivers(drivers), indexs(indexs), model(model){}
  void parseQuery(std::wstring& query);
  void searchPath(std::wstring& path);
  void searchContent(std::wstring& content);
  void display();
  void display(std::set<FileEntry*>& res);
  void filter(std::wstring& newpath);
  void sort();
private:
  QStandardItemModel* model;
  std::wstring addHighLight(std::wstring path);
  std::vector<USNParser*> drivers;
  std::vector<FileIndex*> indexs;
  std::set<FileEntry*> content_result;
  std::set<FileEntry*> path_result;
  std::vector<std::wstring> splited;
};
