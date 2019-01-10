#include "FileIndex.h"
#pragma warning (disable: 4267)

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


FileInfo::FileInfo(FILEREF num, const std::wstring& path) {
	NLPIR_Init();
	FileNum = num;
	FileName = identifyName(path);
	FilePath = path;
	FileContent = Reader::read(path);
	std::string temp = wchar2char(FileContent.c_str());
	std::string LexResult = NLPIR_ParagraphProcess(temp.c_str(), 0);
	std::wstring result = char2wchar(LexResult.c_str());
	NLPIR_Exit();
	max_freq = 0;
	int start_pos;
	bool valid = false;
	bool k = false;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	for (int i = 0; i < result.length(); i++) {
		if (result[i] < 33 || result[i] > 61000) {
			if (valid == true) {
				if (max_freq == 0)
					max_freq = 1;
				fword tpword;
				tpword.word = result.substr(start_pos, i - start_pos);
				tpword.freq = 1;
				bool k = false;
				for (int j = 0; j < words.size(); j++) {
					if (words[j].word == tpword.word) {
						words[j].freq++;
						if (words[j].freq > max_freq)
							max_freq = words[j].freq;
						k = true;
						break;
					}
				}
				if(k == false)
					words.push_back(tpword);
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


FileIndex::FileIndex(USNParser* driver) : driver(driver) {}

void FileIndex::InsertFiles(const std::wstring& dir) {
	auto ref_num = driver->getFileRef(dir);
	std::set<FileEntry*> files;
	driver->recursiveAdd(ref_num, files);
	for (auto file : files) {
		if (Reader::isValid(file->file_name)) {
			file->genPath(driver->all_entries);
			InsertFileIndex(file->file_ref, file->full_path);
		}
	}
}

void FileIndex::InsertFileIndex(FILEREF num, const std::wstring& path) {
	if (exist(num)) DeleteFileIndex(path);
	FileInfo NewFile = FileInfo(num, path);
	bool check = false;
	std::list<post>::iterator iter;
	for (int i = 0; i < NewFile.words.size(); i++) {
		std::wstring word = NewFile.words[i].word;
		post tempPost;
		tempPost.FileNum = NewFile.FileNum;
		tempPost.FreqNum = NewFile.words[i].freq;
		DB[word].push_back(tempPost);
	}
	Files.push_back(NewFile);
}

void FileIndex::DeleteFileIndex(const std::wstring& path) {
	std::list<FileInfo>::iterator iter;
	std::list<post>::iterator iter2;
	for (iter = Files.begin(); iter != Files.end(); ++iter) {
		if (iter->FilePath == path) {
			break;
		}
	}
	for (int i = 0; i < iter->words.size(); i++) {
		for (iter2 = DB[iter->words[i].word].begin(); iter2 != DB[iter->words[i].word].end(); ++iter2) {
			if (iter2->FileNum == iter->FileNum) {
				DB[iter->words[i].word].erase(iter2);
				break;
			}
		}
	}
	Files.erase(iter);
}

bool postcompare(post a, post b) {
	return a.FreqNum > b.FreqNum;
}

bool FileIndex::exist(FILEREF num) {
	for (auto iter = Files.begin(); iter != Files.end(); ++iter)
		if (iter->FileNum == num) return true;
	return false;
}

std::set<FileEntry*> FileIndex::SearchFile(const std::wstring &sentence) {
	NLPIR_Init();
	std::string temp = wchar2char(sentence.c_str());
	std::string LexResult = NLPIR_ParagraphProcess(temp.c_str(), 0);
	std::wstring result = char2wchar(LexResult.c_str());
	NLPIR_Exit();
	int max_freq = 0;
	bool k = false;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	std::vector<fword> words;
	int start_pos;
	bool valid = false;
	std::vector<post> posts;
	std::list<post>::iterator iter;
	std::list<FileInfo>::iterator iter2;
	for (int i = 0; i < result.length(); i++) {
		if (result[i] < 33 || result[i] > 61000) {
			if (valid == true) {
				if (max_freq == 0)
					max_freq = 1;
				fword tpword;
				tpword.word = result.substr(start_pos, i - start_pos);
				tpword.freq = 1;
				bool k = false;
				for (int j = 0; j < words.size(); j++) {
					if (words[j].word == tpword.word) {
						words[j].freq++;
						if (words[j].freq > max_freq)
							max_freq = words[j].freq;
						k = true;
						break;
					}
				}
				if (k == false)
					words.push_back(tpword);
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
		for (iter = DB[words[i].word].begin(); iter != DB[words[i].word].end(); ++iter) {
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
	for (int i = 0; i < words.size(); i++) {
		if (DB[words[i].word].size() == 0) {
			words.erase(words.begin() + i);
			i--;
			continue;
		}
		words[i].tfidf = (0.5 + 0.5 * (double(words[i].freq) / double(max_freq)));
		words[i].tfidf *= double(log10(Files.size() / DB[words[i].word].size()) + 1);
	}
	double d1 = 0;
	for (auto diter = DB.begin(); diter != DB.end(); diter++) {
		if (diter->second.size() == 0)
			continue;
		int k;
		double tp_tfidf;
		for (k = 0; k < words.size(); k++) {
			if (diter->first == words[k].word) {
				tp_tfidf = words[k].tfidf;
				break;
			}
		}
		if (k == words.size()) 
			tp_tfidf = 0.5 * double(log10(Files.size() / diter->second.size()) + 1);

		d1 += tp_tfidf * tp_tfidf;
	}
	for (int i = 0; i < posts.size(); i++) {
		for (auto fiter = Files.begin(); fiter != Files.end(); fiter++) {
			if (posts[i].FileNum == fiter->FileNum) {
				posts[i].FreqNum = 0;
				double d2 = 0;
				for (int j = 0; j < fiter->words.size(); j++) {
					int k;
					double tp_tfidf;
					d2 += fiter->words[j].tfidf * fiter->words[j].tfidf;
					for (k = 0; k < words.size(); k++) {
						if (fiter->words[j].word == words[k].word) {
							tp_tfidf = words[k].tfidf;
							break;
						}
					}
					if (k == words.size())
						tp_tfidf = 0.5 * double(log10(Files.size() / DB[fiter->words[j].word].size()) + 1);
					posts[i].FreqNum += fiter->words[j].tfidf * tp_tfidf;
				}
				posts[i].FreqNum /= sqrt(d1);
				posts[i].FreqNum /= sqrt(d2);
				break;
			}
		}
	}
	sort(posts.begin(), posts.end(), postcompare);
	std::set<FileEntry*> Result;
	for (int i = 0; i < posts.size(); i++) {
		for (iter2 = Files.begin(); iter2 != Files.end(); ++iter2) {
			if (iter2->FileNum == posts[i].FileNum) {
				Result.insert(driver->all_entries[iter2->FileNum]);
				break;
			}
		}
	}
	return Result;
}

void FileIndex::Calctfidf() {
	for (auto iter = Files.begin(); iter != Files.end(); ++iter) {
		for (int i = 0; i < iter->words.size(); i++) {
			iter->words[i].tfidf = (double(iter->words[i].freq) / double(iter->max_freq));
			iter->words[i].tfidf *= double(log10(Files.size() / DB[iter->words[i].word].size()) + 1);
		}
	}
}
