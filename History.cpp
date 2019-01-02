#include "History.h"


void History::addHistory(const std::wstring &path) {
	for (int i = 0; i < his.size(); i++) {
		if (his[i] == path) {
			num[i]++;
			return;
		}
	}
	his.push_back(path);
	num.push_back(1);
}

std::wstring identifyName(const std::wstring& path) {
	for (int i = path.length() - 1; i > 0; i--) {
		if (path[i] == L'\\')
			return path.substr(i + 1);
	}
	return path;
}

std::vector<std::wstring> History::recommend(const std::wstring &path) {
	std::vector<std::wstring> result;
	std::vector<int> res_num;
	std::wstring name = identifyName(path);
	for (int i = 0; i < his.size(); i++) {
		int loc = his[i].find(name);
		if (loc > -1) {
			int j = 0;
			for (j = 0; j < result.size(); j++) {
				if (res_num[j] < num[i])
					break;
			}
			result.insert(result.begin() + j, his[i]);
			res_num.insert(res_num.begin() + j, num[i]);
		}
	}
	return result;
}
