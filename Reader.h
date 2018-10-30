#pragma once
#include <string>
#include <set>
class Reader {
public:
    std::string read(const std::string& path);
private:
    std::set<std::string> plain_text_suffixes = { "txt", "html", "md", "h", "c", "cpp", "hpp", "java" };
    std::set<std::string> doc_suffixes = { "doc", "docx" };
    std::set<std::string> xls_suffixes = { "xls", "xlsx" };
    std::set<std::string> ppt_suffixes = { "ppt", "pptx" };
    std::set<std::string> pdf_suffixes = { "pdf" };
    std::string readFromPlainText(const std::string& path);
        std::string readFromDoc(const std::string& path);
        std::string readFromXls(const std::string& path);
        std::string readFromPPt(const std::string& path);
        std::string readFromPdf(const std::string& path);
    // others
};