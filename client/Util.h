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
		//�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����  
		char * strs = new char[str.length() + 1]; //��Ҫ����  
		strcpy_s(strs, str.length()+1, str.c_str());

		char * d = new char[delim.length() + 1];
		strcpy_s(d, delim.length()+1, delim.c_str());

		char *p = NULL;
		p = strtok_s(strs, d, &next_token);
		while (p != NULL) {
			std::string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
			res.push_back(s); //����������  
			p = strtok_s(NULL, d, &next_token);
		}

		delete strs;
		delete p;

		return res;
	}
};

#endif