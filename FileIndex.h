#pragma once
#include <set>
#include <string>
#include <map>
class FileIndex {
public:
    static FileIndex& Instance();
    FileIndex() {}
    FileIndex(FileIndex const&) = delete;
    void operator=(FileIndex const&) = delete;
    
private:
    void buildIndex(char dist);
    void parseNTFS();
    std::set<std::string> files;
    std::map<std::string, std::string> content;
};
