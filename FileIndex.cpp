#include "FileIndex.h"

wchar_t * char2wchar(const char* cchar) {
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

char * wchar2char(const wchar_t* wchar) {
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}


FileInfo::FileInfo(int num, const std::wstring& path, QProcess *process) {
	NLPIR_Init();
	FileNum = num;
	FileName = identifyName(path);
	FilePath = path;
	Reader reader(process);
	FileContent= reader.read(path);
	std::string temp = wchar2char(FileContent.c_str());
	std::string LexResult = NLPIR_ParagraphProcess(temp.c_str(), 0);
	std::wstring result = char2wchar(LexResult.c_str());
	NLPIR_Exit();
	int start_pos;
	bool valid = false;
	for (int i = 0; i < result.length(); i++) {
		if (result[i] < 33) {
			if (valid == true) {
				words.push_back(result.substr(start_pos, i - start_pos));
			}
			valid = false;
		}
		else {
			if (valid == false) {
				start_pos = i;
			}
			valid = true;
		}
	}
}

std::wstring FileInfo::identifyName(const std::wstring& path) {
	for (int i = path.length() - 1; i > 0; i--) {
		if (path[i] == L'\\')
			return path.substr(i + 1);
	}
	return L"";
}


FileIndex::FileIndex(QProcess *mprocess) {
	process = mprocess;
	FileNum = 0;
}

void FileIndex::InsertFiles(const std::wstring& dir) {
  
}

void FileIndex::InsertFile(const std::wstring& path) {
	FileInfo NewFile = FileInfo(FileNum++, path, process);
	Files.push_back(NewFile);
	bool check = false;
	std::list<post>::iterator iter;
	for (int i = 0; i < NewFile.words.size(); i++) {
		std::wstring word = NewFile.words[i];
		for (iter = DB[word].begin(); iter != DB[word].end(); ++iter) {
			if (iter->FileNum == NewFile.FileNum) {
				iter->FreqNum++;
				check = true;
				break;
			}
		}
		if (check == false) {
			post tempPost;
			tempPost.FileNum = NewFile.FileNum;
			tempPost.FreqNum = 1;
			DB[word].push_back(tempPost);
		}
		check = false;
	}
}

void FileIndex::DeleteFile(const std::wstring& path) {
	std::list<FileInfo>::iterator iter;
	std::list<post>::iterator iter2;
	for (iter = Files.begin(); iter != Files.end(); iter++) {
		if (iter->FilePath == path) {
			break;
		}
	}
	for (int i = 0; i < iter->words.size(); i++) {
		for (iter2 = DB[iter->words[i]].begin(); iter2 != DB[iter->words[i]].end(); iter2++) {
			if (iter2->FileNum == iter->FileNum) {
				DB[iter->words[i]].erase(iter2);
				break;
			}
		}
	}
	Files.erase(iter);
}

bool postcompare(post a, post b) {
	return a.FreqNum > b.FreqNum;
}

std::vector<FileInfo> FileIndex::SearchFile(const std::wstring &sentence) {
	NLPIR_Init();
	std::string temp = wchar2char(sentence.c_str());
	std::string LexResult = NLPIR_ParagraphProcess(temp.c_str(), 0);
	std::wstring result = char2wchar(LexResult.c_str());
	NLPIR_Exit();
	std::vector<std::wstring> words;
	int start_pos;
	bool valid = false;
	std::vector<post> posts;
	std::list<post>::iterator iter;
	std::list<FileInfo>::iterator iter2;
	for (int i = 0; i < result.length(); i++) {
		if (result[i] < 33) {
			if (valid == true) {
				words.push_back(result.substr(start_pos, i - start_pos));
			}
			valid = false;
		}
		else {
			if (valid == false) {
				start_pos = i;
			}
			valid = true;
		}
	}
	bool check = false;
	for (int i = 0; i < words.size(); i++) {
		for (iter = DB[words[i]].begin(); iter != DB[words[i]].end(); ++iter) {
			for (int k = 0; k < posts.size(); k++) {
				if (iter->FileNum == posts[k].FileNum) {
					posts[k].FreqNum += iter->FreqNum;
					check = true;
					break;
				}
			}
			if (check == false) {
				posts.push_back(*iter);
			}
			check = false;
		}
	}
	sort(posts.begin(), posts.end(), postcompare);
	std::vector<FileInfo> Result;
	for (int i = 0; i < posts.size(); i++) {
		for (iter2 = Files.begin(); iter2 != Files.end(); iter2++) {
			if (iter2->FileNum == posts[i].FileNum) {
				Result.push_back(*iter2);
				break;
			}
		}
	}
	return Result;
}
