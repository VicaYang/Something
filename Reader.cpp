#include "Reader.h"
#include <QDebug>
#pragma warning (disable: 4267)

std::set<std::wstring> Reader::all{ L"txt", L"md", L"h", L"c", L"cpp", L"hpp", L"java",
L"doc", L"docx", L"xls", L"xlsx", L"ppt", L"pptx", L"pdf", L"html" };
std::set<std::wstring> Reader::plain_text_suffixes{ L"txt", L"md", L"h" L"c", L"cpp", L"hpp", L"java" };
std::set<std::wstring> Reader::MS_suffixes{ L"doc", L"docx", L"xls", L"xlsx", L"ppt", L"pptx" };
std::set<std::wstring> Reader::pdf_suffixes{ L"pdf" };

std::wstring Reader::read(const std::wstring& path) {
	std::wstring format = identifyFormat(path);

	if (plain_text_suffixes.count(format)) {
		return readFromPlainText(path);
	}
	else if (MS_suffixes.count(format)) {
		return readFromMS(path);
	}
	else if (pdf_suffixes.count(format)) {
		return readFromPdf(path);
	}
	else {
		return L"";
	}
}

std::wstring Reader::identifyFormat(const std::wstring& path) {
	for (int i = path.length() - 1; i > 0; i--) {
		if (path[i] == L'.')
			return path.substr(i+1);
	}
	return L"";
}

bool Reader::isValid(const std::wstring& path) {
  auto format = identifyFormat(path);
  return all.count(format) == 1;
}

std::wstring Reader::readFromPlainText(const std::wstring& path) {
	int len;
	wchar_t temp[500] = {0,};
	std::wstring result = L"";
	FILE *fp = _wfopen(path.c_str(), L"r+,ccs=UTF-8");
  if (!fp) return result;
	while (!feof(fp)) {
		len = fread(temp, 1, sizeof(temp) - 2, fp);
		temp[len / 2] = 0;
		result = result + temp;
	}
	fclose(fp);
	return result;
}

std::wstring Reader::readFromMS(const std::wstring& path) {
	std::wstring cmd = L"doctotext\\doctotext.exe ";
	cmd = cmd + path;
	QString qcmd = QString::fromStdWString(cmd);
  auto process = new QProcess();
	process->start(qcmd);
	process->waitForFinished();
	QString output(process->readAllStandardOutput());
	process->close();
	std::wstring result = output.toStdWString();
  delete process;
	return result;
}
std::wstring Reader::readFromPdf(const std::wstring& path) {
	std::wstring cmd = L"xpdf\\bin64\\pdftotext.exe ";
	cmd = cmd + path + L" temp.txt";
	QString qcmd = QString::fromStdWString(cmd);
	auto process = new QProcess();
	process->start(qcmd);
	process->waitForFinished();
	process->close();
	int len;
	wchar_t temp[500] = { 0, };
	std::wstring tpresult = L"";
	FILE *fp = _wfopen(L"temp.txt", L"r+,ccs=UTF-8");
	while (!feof(fp)) {
		len = fread(temp, 1, sizeof(temp), fp);
		temp[len / 2 - 1] = 0;
		tpresult = tpresult + temp;
	}
	fclose(fp);
	process->close();
	delete process;
	int check = 0;
	std::wstring result = L"";
	for (int i = 0; i < tpresult.length(); i++) {
		if (tpresult[i] > 32 && tpresult[i] < 61000) {
			result += tpresult[i];
			check = 0;
		}
		else {
			if (check > 0)
				continue;
			result += 32;
			check++;
		}
	}
	return result;
}
