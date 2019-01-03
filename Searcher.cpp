#include "Searcher.h"
#include "util.h"
#include <queue>
#include <sstream>

void Searcher::parseQuery(std::wstring& query) {
  content_result.clear();
  path_result.clear();
  splited.clear();
  size_t loc = query.find(L"\\content:");
  if (loc != std::wstring::npos) {
    _content = query.substr(loc + 9);
    searchContent(_content);
  }
  _path = loc == std::wstring::npos ? query : query.substr(0, loc);
  if (_path.size() > 0)
    searchPath(_path);
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
  auto iter = path_result.begin();
  while (iter != path_result.end()) {
    if ((*iter)->full_path.find(newpath) == std::wstring::npos) {
      iter = path_result.erase(iter);
    } else {
      ++iter;
    }
  }
}

std::wstring Searcher::addHighLight(std::wstring& path) {
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

bool Searcher::singleSearch(std::wstring& path, std::wstring& query_path) {
  if (query_path.size() == 0) return false;
  splited = strsplit(query_path);
  for (auto& word : splited) {
    if (path.find(word) == std::wstring::npos) return false;
  }
  return true;
}

bool Searcher::update(FileEntry* entry, UpdateType type) {
  if (type == UpdateType::ADD) {
    content_result.clear();
    searchContent(_content);
    if (singleSearch(entry->full_path, _path)) {
      path_result.insert(entry);
    }
    return true;
  }
  if (type == UpdateType::REMOVE) {
    content_result.clear();
    searchContent(_content);
    auto iter = path_result.find(entry);
    if (iter != path_result.end()) {
      path_result.erase(iter);
    }
    return true;
  }
  if (type == UpdateType::CONTENT_CHANGE) {
    content_result.clear();
    searchContent(_content);
    return true;
  }
  return false;
}

bool Searcher::recvPUSN(int id, PUSN_RECORD pusn) {
  auto& sub_entries = drivers[id]->sub_entries;
  auto& all_entries = drivers[id]->all_entries;
  auto& index = indexs[id];
  auto& recycle = drivers[id]->recycle;
  if (pusn->Reason == USN_REASON_RENAME_OLD_NAME) { // remove link
    auto& childs = sub_entries[pusn->ParentFileReferenceNumber];
    auto iter = childs.begin();
    while (iter != childs.end()) {
      if ((*iter)->file_ref == pusn->FileReferenceNumber) break;
      ++iter;
    }
    auto ptr = *iter;
    ptr->genPath(all_entries);
    recycle[pusn->FileReferenceNumber] = ptr;
    if (index->exist(pusn->FileReferenceNumber)) index->DeleteFileIndex(ptr->full_path);
    childs.erase(iter);
    auto iter2 = all_entries.find(pusn->FileReferenceNumber);
    all_entries.erase(iter2);
    return update(ptr, UpdateType::REMOVE);
  }
  if (pusn->Reason == USN_REASON_RENAME_NEW_NAME) { // add link
    auto file_entry = recycle[pusn->FileReferenceNumber];
    file_entry->parent_ref = pusn->ParentFileReferenceNumber;
    file_entry->file_name = std::wstring(pusn->FileName).substr(0, pusn->FileNameLength / 2);
    file_entry->full_path.clear();
    all_entries.insert({ pusn->FileReferenceNumber, file_entry });
    if (sub_entries.count(pusn->ParentFileReferenceNumber) == 0)
      sub_entries.insert({ pusn->ParentFileReferenceNumber , std::vector<FileEntry*>() });
    sub_entries[pusn->ParentFileReferenceNumber].push_back(file_entry);
    file_entry->genPath(all_entries);
    auto iter = recycle.find(pusn->FileReferenceNumber);
    recycle.erase(iter);
    if (Reader::isValid(file_entry->full_path)) index->InsertFileIndex(file_entry->file_ref, file_entry->full_path);
    return update(file_entry, UpdateType::ADD);
  }
  if (pusn->Reason == USN_REASON_FILE_CREATE) {
    auto ptr = new FileEntry(pusn);
    all_entries.insert({ pusn->FileReferenceNumber, ptr });
    if (sub_entries.count(pusn->ParentFileReferenceNumber) == 0)
      sub_entries.insert({ pusn->ParentFileReferenceNumber , std::vector<FileEntry*>() });
    sub_entries[pusn->ParentFileReferenceNumber].push_back(ptr);
    ptr->genPath(all_entries);
    if (Reader::isValid(ptr->full_path)) index->InsertFileIndex(ptr->file_ref, ptr->full_path);
    return update(ptr, UpdateType::ADD);
  }
  if (pusn->Reason & (0xff | USN_REASON_OBJECT_ID_CHANGE)) {
    auto file_entry = all_entries[pusn->FileReferenceNumber];
    file_entry->genPath(drivers[id]->all_entries);
    index->DeleteFileIndex(file_entry->full_path);
    index->InsertFileIndex(file_entry->file_ref, file_entry->full_path);
    return update(file_entry, UpdateType::CONTENT_CHANGE);
  }
  return false;
}
