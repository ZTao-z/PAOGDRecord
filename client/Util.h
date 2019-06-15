#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <vector>
#include <string.h>

class Utils {
public:
	static std::vector<std::string> split(const std::string& str, const std::string& delim) {
		std::vector<std::string> res;
		char *next_token = NULL;
		if ("" == str) return res;
		//先将要切割的字符串从string类型转换为char*类型  
		char * strs = new char[str.length() + 1]; //不要忘了  
		strcpy_s(strs, str.length()+1, str.c_str());

		char * d = new char[delim.length() + 1];
		strcpy_s(d, delim.length()+1, delim.c_str());

		char *p = NULL;
		p = strtok_s(strs, d, &next_token);
		while (p != NULL) {
			std::string s = p; //分割得到的字符串转换为string类型  
			res.push_back(s); //存入结果数组  
			p = strtok_s(NULL, d, &next_token);
		}

		delete strs;
		delete p;

		return res;
	}
};

#endif