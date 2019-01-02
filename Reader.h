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
	static std::wstring read(const std::wstring& path);
  static bool isValid(const std::wstring& path);

private:
  static std::set<std::wstring> all;
	static std::set<std::wstring> plain_text_suffixes;
	static std::set<std::wstring> MS_suffixes;
  static std::set<std::wstring> pdf_suffixes;
	static std::set<std::wstring> html_suffixes;
	static std::wstring readFromPlainText(const std::wstring& path);
	static std::wstring readFromMS(const std::wstring& path);
	static std::wstring readFromPdf(const std::wstring& path);
	static std::wstring identifyFormat(const std::wstring& path);
};


