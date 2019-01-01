#include "Reader.h"
#include <QDebug>
#pragma warning (disable 4267)

std::set<std::wstring> Reader::all{ L"txt", L"md", L"h", L"c", L"cpp", L"hpp", L"java",
L"doc", L"docx", L"xls", L"xlsx", L"ppt", L"pptx", L"pdf", L"html" };
std::set<std::wstring> Reader::plain_text_suffixes{ L"txt", L"md", L"h" L"c", L"cpp", L"hpp", L"java" };
std::set<std::wstring> Reader::doc_suffixes{ L"doc", L"docx" };
std::set<std::wstring> Reader::xls_suffixes{ L"xls", L"xlsx" };
std::set<std::wstring> Reader::ppt_suffixes{ L"ppt", L"pptx" };
std::set<std::wstring> Reader::pdf_suffixes{ L"pdf" };
std::set<std::wstring> Reader::html_suffixes{ L"html" };

Reader::Reader(QProcess *mprocess) {
	process = mprocess;
}

std::wstring Reader::read(const std::wstring& path) {
	std::wstring format = identifyFormat(path);

	if (plain_text_suffixes.count(format)) {
		return readFromPlainText(path);
	}
	else if (doc_suffixes.count(format)) {
		return readFromDoc(path);
	}
	else if (xls_suffixes.count(format)) {
		return readFromPlainText(path);
	}
	else if (ppt_suffixes.count(format)) {
		return readFromPlainText(path);
	}
	else if (pdf_suffixes.count(format)) {
		return readFromPdf(path);
	}
	else if (html_suffixes.count(format)) {
		return readFromHtml(path);
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
	while (!feof(fp)) {
		len = fread(temp, 1, sizeof(temp), fp);
		temp[len / 2 - 1] = 0;
		result = result + temp;
	}
	fclose(fp);
	return result;
}

std::wstring Reader::readFromDoc(const std::wstring& path) {
	std::wstring cmd = L"doctotext\\doctotext\\doctotext.exe ";
	cmd = cmd + path;
	QString qcmd = QString::fromStdWString(cmd);
	process->start(qcmd);
	process->waitForFinished();
	QString output(process->readAllStandardOutput());
	process->close();
	std::wstring result = output.toStdWString();
	return result;
}

std::wstring Reader::readFromPPt(const std::wstring& path) {
	std::wstring cmd = L"doctotext\\doctotext\\doctotext.exe ";
	cmd = cmd + path;
	QString qcmd = QString::fromStdWString(cmd);
	process->start(qcmd);
	process->waitForFinished();
	QString output(process->readAllStandardOutput());
	process->close();
	std::wstring result = output.toStdWString();
	return result;
}

std::wstring Reader::readFromXls(const std::wstring& path) {
	std::wstring cmd = L"doctotext\\doctotext\\doctotext.exe ";
	cmd = cmd + path;
	QString qcmd = QString::fromStdWString(cmd);
	process->start(qcmd);
	process->waitForFinished();
	QString output(process->readAllStandardOutput());
	process->close();
	std::wstring result = output.toStdWString();
	return result;
}

std::wstring Reader::readFromPdf(const std::wstring& path) {
	std::wstring cmd = L"xpdf\\bin64\\pdftotext.exe ";
	cmd = cmd + path + L" temp.txt";
	QString qcmd = QString::fromStdWString(cmd);
	process->start(qcmd);
	process->waitForFinished();
	process->close();
	int len;
	wchar_t temp[500] = { 0, };
	std::wstring result = L"";
	FILE *fp = _wfopen(L"temp.txt", L"r+,ccs=UTF-8");
	while (!feof(fp)) {
		len = fread(temp, 1, sizeof(temp), fp);
		temp[len / 2 - 1] = 0;
		result = result + temp;
	}
	fclose(fp);
	process->close();
	return result;
}

std::wstring Reader::readFromHtml(const std::wstring& path) {
	std::wstring cmd = L"doctotext\\doctotext.exe ";
	cmd = cmd + path;
	QString qcmd = QString::fromStdWString(cmd);
	process->start(qcmd);
	process->waitForFinished();
	QString output(process->readAllStandardOutput());
	process->close();
	std::wstring result = output.toStdWString();
	return result;
}
