#include "Searcher.h"
#include "util.h"
#include <queue>
#include <sstream>

void Searcher::parseQuery(std::wstring& query) {
  content_result.clear();
  path_result.clear();
  if (query.size() < 3) return;
  size_t loc = query.find(L"\\content:");
  if (loc != std::wstring::npos) {
    auto content = query.substr(loc + 9);
    searchContent(content);
  }
  if (loc > 3) {
    auto path = loc == std::wstring::npos ? query : query.substr(0, loc);
    if (path.size() > 3) {
      searchPath(path);
    }
  }
  display();
}

void Searcher::searchPath(std::wstring& path) {
  path_result.clear();
  splited = strsplit(path);
  if (splited.empty()) return;
  for (auto driver : drivers) {
    std::set<FileEntry*> tmp = driver->query(splited[0]);
    for (size_t i = 1; i < splited.size(); ++i) {
      auto _tmp = driver->query(splited[i]);
      auto it1 = tmp.begin();
      auto it2 = _tmp.begin();
      while ((it1 != tmp.end()) && (it2 != _tmp.end())) {
        if (*it1 < *it2) { 
          tmp.erase(it1++);
        } else if (*it2 < *it1) {
          ++it2;
        } else {
          ++it1;
          ++it2;
        }
      }
      tmp.erase(it1);
    }
    std::copy(tmp.begin(), tmp.end(), std::inserter(path_result, path_result.end()));
  }
}

void Searcher::searchContent(std::wstring& content) {
  content_result.clear();
  for (auto index : indexs) {
    auto tmp_res = index->SearchFile(content);
    std::copy(tmp_res.begin(), tmp_res.end(), std::inserter(content_result, content_result.end()));
  }
}

void Searcher::filter(std::wstring& newpath) {
  for (auto iter = path_result.begin(); iter != path_result.end(); ++iter) {
    if ((*iter)->full_path.find(newpath) == std::wstring::npos) {
      iter = path_result.erase(iter);
    } else {
      ++iter;
    }
  }
  display();
}

void Searcher::sort() {
  display();
}

void Searcher::display() {
  std::set<FileEntry*> tmp;
  if (!content_result.empty() && !path_result.empty()) {
    std::set_intersection(content_result.begin(), content_result.end(), path_result.begin(), path_result.end(), std::inserter(tmp, tmp.end()));
    display(tmp);
  } else {
    if (!content_result.empty()) display(content_result);
    else if (!path_result.empty()) display(path_result);
  }
}

std::wstring Searcher::addHighLight(std::wstring path) {
  std::wstringstream ss;
  std::wstring res;
  std::priority_queue<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>, std::greater<std::pair<size_t, size_t>>> PQ;
  for (size_t i = 0; i <splited.size(); ++i) {
    size_t loc = path.find(splited[i]);
    while (loc != std::wstring::npos) {
      PQ.push( std::pair<size_t, size_t>(loc, splited[i].length()));
      loc = path.find(splited[i], loc + 1);
    }
  }
  int cur = 0;
  while (!PQ.empty()) {
    auto top = PQ.top();
    PQ.pop();
    if (top.first < cur) continue;
    ss << path.substr(cur, top.first - cur);
    ss << L"<b>";
    ss << path.substr(top.first, top.second);
    ss << L"</b>";
    cur = top.first + top.second;
  }
  ss << path.substr(cur);
  return ss.str();
}

void Searcher::display(std::set<FileEntry*>& res) {
  model->removeRows(0, model->rowCount());
  int i = 0;
  for (auto ptr : res) {
    model->setItem(i, 0, new QStandardItem(QString::fromStdWString(addHighLight(ptr->file_name))));
    model->setItem(i, 1, new QStandardItem(QString::fromStdWString(addHighLight(ptr->full_path))));
    i++;
  }
}