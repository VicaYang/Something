#include "FileIndex.h"

wchar_t * char2wchar(const char* cchar){
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

char * wchar2char(const wchar_t* wchar)
{
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}


FileInfo::FileInfo(int num, const std::wstring& path) {
	NLPIR_Init();
	FileNum = num;
	FileName = path;
	FilePath = path;
	Reader reader;
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

FileIndex::FileIndex() {
	FileNum = 0;
}

void FileIndex::InsertFile(const std::wstring& path){
	FileInfo NewFile = FileInfo(FileNum++, path);
	Files.push_back(NewFile);
	bool check = false;
	for (int i = 0; i < NewFile.words.size(); i++) {
		std::wstring word = NewFile.words[i];
		for (int j = 0; j < DB[word].size(); j++) {
			if (DB[word][j].FileNum == NewFile.FileNum) {
				DB[word][j].FreqNum++;
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
		for (int j = 0; j < DB[words[i]].size(); j++) {
			for (int k = 0; k < posts.size(); k++) {
				if (DB[words[i]][j].FileNum == posts[k].FileNum) {
					posts[k].FreqNum += DB[words[i]][j].FreqNum;
					check = true;
					break;
				}
			}
			if (check == false) {
				posts.push_back(DB[words[i]][j]);
			}
			check = false;
		}
	}
	sort(posts.begin(), posts.end(), postcompare);
	std::vector<FileInfo> Result;
	for (int i = 0; i < posts.size(); i++) {
		Result.push_back(Files[posts[i].FileNum]);
	}
	return Result;
}
