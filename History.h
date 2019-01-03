#pragma once
#include <vector>

class History {
public:
	std::vector<std::wstring> his;
	std::vector<int> num;
	void addHistory(const std::wstring &path);
	std::vector<std::wstring> recommend(const std::wstring &path);
};
