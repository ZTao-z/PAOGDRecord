#ifndef GAME_NETWORK_H
#define GAME_NETWORK_H

#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>

#include <deque>
#include <vector>

#include "donghua_gameStruct.h"
#include "Util.h"

#pragma comment(lib, "WS2_32")

using namespace std;

class GameNetwork {
private:
	string IP;
	int PORT;
	string playerName;
	std::map<std::string, std::string> old_actionList;
public:
	string gameInitialPosition;
	std::map<std::string, std::string> todo_actionList;

	GameNetwork(string name, string ip = "127.0.0.1", int port = 1000) {
		this->playerName = name;
		this->IP = ip;
		this->PORT = port;
	}

	GameNetwork() {

	}

	~GameNetwork() {

	}

	vector<string> split(const string& str, const string& delim) {
		vector<string> res;
		char *next_token = NULL;
		if ("" == str) return res;
		//先将要切割的字符串从string类型转换为char*类型  
		char * strs = new char[str.length() + 1]; //不要忘了  
		strcpy_s(strs, str.length() + 1, str.c_str());

		char * d = new char[delim.length() + 1];
		strcpy_s(d, str.length() + 1, delim.c_str());

		char *p = strtok_s(strs, d, &next_token);
		while (p) {
			string s = p; //分割得到的字符串转换为string类型  
			res.push_back(s); //存入结果数组  
			p = strtok_s(NULL, d, &next_token);
		}

		delete strs;
		delete p;

		return res;
	}

	bool OpenGame() {
		WSADATA data;
		WORD w = MAKEWORD(2, 0);
		::WSAStartup(w, &data);

		SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(this->PORT);

		inet_pton(AF_INET, (this->IP).c_str(), &destAddr.sin_addr);//destAddr.sin_addr.S_un.S_addr = inet_addr((this->IP).c_str());

		string helloMsg = this->playerName + ":" + "h";

		sendto(s, helloMsg.c_str(), 60, 0, (sockaddr*)&destAddr, sizeof(destAddr));

		int n = sizeof(destAddr);
		bool isLogIn = false;

		char buffer[21] = { 0 };
		int c = recvfrom(s, buffer, 20, 0, (sockaddr*)&destAddr, &n);
		if (c != SOCKET_ERROR && buffer) {
			std::string res = buffer;
			if (res == "O") {
				isLogIn = true;
			}
		}

		closesocket(s);
		WSACleanup();

		return isLogIn;
	}

	bool beginGame(std::string hostname) {
		WSADATA data;
		WORD w = MAKEWORD(2, 0);
		::WSAStartup(w, &data);

		SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(this->PORT);

		inet_pton(AF_INET, (this->IP).c_str(), &destAddr.sin_addr);//destAddr.sin_addr.S_un.S_addr = inet_addr((this->IP).c_str());

		string beginMsg = this->playerName + ":" + "r";

		sendto(s, beginMsg.c_str(), 60, 0, (sockaddr*)&destAddr, sizeof(destAddr));

		if (hostname == "player1") {
			beginMsg = this->playerName + ":" + "s";
			string answer = "N";
			cout << "Start Game?(Y/N): ";
			while (cin >> answer) {
				if (answer == "Y") {
					break;
				}
				else {
					cout << "Start Game?(Y/N): ";
				}
			}
			sendto(s, beginMsg.c_str(), 60, 0, (sockaddr*)&destAddr, sizeof(destAddr));
		}
		else {
			std::cout << "Waiting to start..." << std::endl;
		}
		
		int n = sizeof(destAddr);
		bool isBegin = false;

		char buffer[1001] = { 0 };
		int c = recvfrom(s, buffer, 20, 0, (sockaddr*)&destAddr, &n);
		if (c != SOCKET_ERROR && buffer) {
			std::string res = buffer;
			if (res == "S") {
				isBegin = true;
			}
		}

		c = recvfrom(s, buffer, 1000, 0, (sockaddr*)&destAddr, &n);
		if (c != SOCKET_ERROR && buffer) {
			std::string res = buffer;
			std::cout << res << std::endl;
			this->gameInitialPosition = res;
		}

		closesocket(s);
		WSACleanup();

		return isBegin;
	}

	void sendAlive() {
		WSADATA data;
		WORD w = MAKEWORD(2, 0);
		::WSAStartup(w, &data);

		SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(this->PORT);

		inet_pton(AF_INET, (this->IP).c_str(), &destAddr.sin_addr);

		string aliveMsg = this->playerName + ":" + "l";

		sendto(s, aliveMsg.c_str(), 60, 0, (sockaddr*)&destAddr, sizeof(destAddr));

		closesocket(s);
		WSACleanup();
	}

	void sendMovement(int movement, int nplayers = 1) {
		string str = "";
		char s[60];
		sprintf_s(s, "%s:%d", this->playerName.c_str(), movement);
		str = s;

		WSADATA data;
		WORD w = MAKEWORD(2, 0);
		::WSAStartup(w, &data);

		SOCKET sct = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(this->PORT);
		inet_pton(AF_INET, (this->IP).c_str(), &destAddr.sin_addr);//destAddr.sin_addr.S_un.S_addr = inet_addr((this->IP).c_str());

		Sleep(rand() % 20);

		sendMsg(sct, destAddr, str.c_str());

		std::map<std::string, std::string> actionList;
		
		int timeout = 3000;
		int b = setsockopt(sct, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

		recvMsg(sct, destAddr, actionList, nplayers);

		for (std::map<std::string, std::string>::iterator ptr = actionList.begin(); ptr != actionList.end(); ptr++) {
			//int s = old_actionList[ptr->first].size();
			//if (s >= ptr->second.size()) break;
			todo_actionList.insert_or_assign(ptr->first, ptr->second/*ptr->second.substr(s)*/);
			std::cout << ptr->first << ":" << todo_actionList[ptr->first] << std::endl;
		}

		this->old_actionList = actionList;

		::closesocket(sct);
		::WSACleanup();
	}

	void recvMsg(SOCKET& s, sockaddr_in& destAddr, std::map<std::string,std::string>& actionList, int nplayers) {
		int n = sizeof(destAddr);
		char buffer[1461] = { 0 };
		int c = recvfrom(s, buffer, 1460, 0, (sockaddr*)&destAddr, &n);
		if (c != SOCKET_ERROR && buffer) {
			std::string res = buffer;
			std::vector<std::string> r = Utils::split(res, ";");
			for (int i = 0; i < r.size(); i++) {
				std::vector<std::string> r_t = Utils::split(r[i], ":");
				if (r_t.size() == 2) {
					actionList.insert_or_assign(r_t[0], r_t[1]);
				}
			}
		}
		/*
		for (int i = 0; i < nplayers; i++) {
			int c = recvfrom(s, buffer, 1460, 0, (sockaddr*)&destAddr, &n);
			if (c != SOCKET_ERROR && buffer) {
				std::string res = buffer;
				std::vector<std::string> r = Utils::split(res, ":");
				actionList.insert_or_assign(r[0], r[1]);
			}
		}
		*/
	}

	void sendMsg(SOCKET& s, sockaddr_in& destAddr, const char* msg) {
		::sendto(s, msg, 1460, 0, (sockaddr*)&destAddr, sizeof(destAddr));
		cout << "Finish!" << endl;
	}
};
#endif