#include "Reader.h"
std::string Reader::read(const std::string& path) {
    if (plain_text_suffixes.count(path)) {
        return readFromPlainText(path);
    }
    if (doc_suffixes.count(path)) {
        return readFromPlainText(path);
    }
    if (pdf_suffixes.count(path)) {
        return readFromPlainText(path);
    }
    return "";
}
std::string Reader::readFromPlainText(const std::string& path) {
    return "";
}
std::string Reader::readFromDoc(const std::string& path) {
    return "";
}
std::string Reader::readFromPPt(const std::string& path) {
    return "";
}
std::string Reader::readFromXls(const std::string& path) {
    return "";
}
std::string Reader::readFromPdf(const std::string& path) {
    return "";
}