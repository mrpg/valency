// Stolen from my C++ snippets: http://mrpg.pw/cpp-snippets
// License: Public Domain
// Max R. P. Grossmann, 2013.

#include <string>

template <class SortedContainer, class Element>
inline bool lexicfind(const SortedContainer& sc, const Element& el) {
	for (auto& cur: sc) {
		if (cur >= el) {
			if (cur == el) {
				return true;
			}
			else {
				return false;
			}
		}
	}
 
	return false;
}

void ltrimhere(std::string& str, std::string sorted_charlist = "\x0009\x000A\x000B\x000C\x000D\x0020\x0085") {
	while (!str.empty()) {
		if (lexicfind(sorted_charlist,str.front())) {
			str.erase(0,1);
		}
		else {
			return;
		}
	}
}

void rtrimhere(std::string& str, std::string sorted_charlist = "\x0009\x000A\x000B\x000C\x000D\x0020\x0085") {
	while (!str.empty()) {
		if (lexicfind(sorted_charlist,str.back())) {
			str.erase(str.end()-1);
		}
		else {
			return;
		}
	}
}

void trimhere(std::string& str) {
	ltrimhere(str);
	rtrimhere(str);
}

void trimhere(std::string& str, std::string sorted_charlist) {
	ltrimhere(str,sorted_charlist);
	rtrimhere(str,sorted_charlist);
}
