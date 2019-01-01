#pragma once
#include <string>
#include <set>
#include <stdlib.h>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <qprocess.h>
#include <qstringlist.h>
#include <qstring.h>

class Reader {
public:
	std::wstring read(const std::wstring& path);
	QProcess *process;
	Reader(QProcess *mprocess);
  static bool isValid(const std::wstring& path);

private:
  static std::set<std::wstring> all;
	static std::set<std::wstring> plain_text_suffixes;
	static std::set<std::wstring> doc_suffixes;
	static std::set<std::wstring> xls_suffixes;
	static std::set<std::wstring> ppt_suffixes;
  static std::set<std::wstring> pdf_suffixes;
	static std::set<std::wstring> html_suffixes;
	std::wstring readFromPlainText(const std::wstring& path);
	std::wstring readFromDoc(const std::wstring& path);
	std::wstring readFromXls(const std::wstring& path);
	std::wstring readFromPPt(const std::wstring& path);
	std::wstring readFromPdf(const std::wstring& path);
	std::wstring readFromHtml(const std::wstring& path);
	static std::wstring identifyFormat(const std::wstring& path);
};


