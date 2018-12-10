#pragma once
#include <string>
#include <set>
#include <stdlib.h>
#include <vector>
#include <windows.h>
#include <stdio.h>

class Reader {
public:
	std::wstring read(const std::wstring& path);
private:
	std::set<std::wstring> plain_text_suffixes = { L"txt", L"html", L"md", L"h", L"c", L"cpp", L"hpp", L"java" };
	std::set<std::wstring> doc_suffixes = { L"doc", L"docx" };
	std::set<std::wstring> xls_suffixes = { L"xls", L"xlsx" };
	std::set<std::wstring> ppt_suffixes = { L"ppt", L"pptx" };
	std::set<std::wstring> pdf_suffixes = { L"pdf" };
	std::set<std::wstring> html_suffixes = { L"html" };
	std::wstring readFromPlainText(const std::wstring& path);
	std::wstring readFromDoc(const std::wstring& path);
	std::wstring readFromXls(const std::wstring& path);
	std::wstring readFromPPt(const std::wstring& path);
	std::wstring readFromPdf(const std::wstring& path);
	std::wstring readFromHtml(const std::wstring& path);
	std::wstring identifyFormat(const std::wstring& path);
};
