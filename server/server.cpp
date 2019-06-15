#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <map>
#include <string.h>

#include <process.h>
//#include <mutex>

#pragma comment(lib, "WS2_32")

using namespace std;

std::map<std::string, std::string> unprocessList;

std::map<std::string, bool> aliveStatus;

bool isStart = false;
int foodSeed = rand() % 100 + 1;

vector<string> split(const string& str, const string& delim) {
	vector<string> res;
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

void show() {
	for (std::map<std::string, std::string>::iterator ptr = unprocessList.begin(); ptr != unprocessList.end(); ptr++) {
		cout << ptr->first << ": " << ptr->second << endl;
	}
}

void initialInfo(string f_name) {
	std::string p = "1";
	unprocessList.insert_or_assign(f_name, p);
}

void updateInfo(string f_name, char movement) {
	unprocessList[f_name] = movement;
}

void sendBack(string f_name, SOCKET& s, sockaddr_in& clientAddr) {
	string total = "";
	for (std::map<std::string, std::string>::iterator ptr = unprocessList.begin(); ptr != unprocessList.end(); ptr++) {
		if (true || ptr->first != f_name) {
			string info = ptr->first + ":" + ptr->second + ";";
			total += info;
		}
	}
	::sendto(s, total.c_str(), 1460, 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
}

int main() {
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	WSAStartup(w, &data);
	
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in hostAddr;
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(1000);
	//hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "127.0.0.1", &hostAddr.sin_addr);
	bind(s, (sockaddr*)&hostAddr, sizeof(hostAddr));
	cout << "UDP Service is running\n";

	map<string, sockaddr_in> addrList;
	vector<string> playerList;

	int count = 0;
	int foodCount = 0;
	int timeout = 2000;
	int b = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

	while (true) {
		sockaddr_in clientAddr;
		int n = sizeof(clientAddr);
		char buffer[1460] = { 0 };
		if (recvfrom(s, buffer, 1460, 0, (sockaddr*)&clientAddr, &n) != SOCKET_ERROR && buffer) {
			string input = buffer;
			vector<string> res = split(input, ":");
			printf("%s : %s\n", res[0].c_str(), res[1].c_str());
			if (res[1][0] == 'h') {
				if (isStart) {
					::sendto(s, "R", 20, 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
				}
				else {
					::sendto(s, "O", 20, 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
					initialInfo(res[0]);
				}
			}
			else if (res[1][0] == 'r') {
				addrList.insert_or_assign(res[0], clientAddr);
				aliveStatus.insert_or_assign(res[0], true);
				playerList.push_back(res[0]);
			}
			else if (res[1][0] == 's') {
				isStart = true;
				for (map<string, sockaddr_in>::iterator i = addrList.begin(); i != addrList.end(); i++) {
					::sendto(s, "S", 20, 0, (sockaddr*)&i->second, sizeof(i->second));
					string t = "";
					for (int j = 0; j < playerList.size(); j++) {
						char s[20];
						sprintf_s(s, 20, "%s,%d,%d", playerList[j].c_str(), j * 2, j * 2);
						t += s;
						if (j < playerList.size() - 1) {
							t += ";";
						}
					}
					::sendto(s, t.c_str(), 1000, 0, (sockaddr*)&i->second, sizeof(i->second));
				}
			}
			else if (res[1][0] == 'l') {
				aliveStatus.insert_or_assign(res[0], true);
			}
			else if (res[1][0] == 'f') {
				char n[5];
				sprintf_s(n, 5, "%d", foodSeed);
				//std::string t = n;
				sendto(s, n, 1460, 0, (sockaddr*)& clientAddr, sizeof(clientAddr));
				foodCount++;
				if (foodCount == playerList.size()) {
					foodSeed = rand() % 100 + 1;
					foodCount = 0;
				}
			}
			else {
				updateInfo(res[0], res[1][0]);
				addrList[res[0]] = clientAddr;
				count++;
				if (count == playerList.size()) {
					for (map<string, sockaddr_in>::iterator i = addrList.begin(); i != addrList.end(); i++) {
						sendBack(i->first, s, i->second);
						/*if (aliveStatus[i->first]) {
							sendBack(i->first, s, i->second);
							aliveStatus[i->first] = true;
						}
						else {
							aliveStatus[i->first] = false;
						}*/
					}
					count = 0;
				}	
			}
		}
		else {
			/*
			for (std::vector<std::string>::iterator ptr = playerList.begin(); ptr != playerList.end(); ) {
				if (!aliveStatus[*ptr]) {
					ptr = playerList.erase(ptr);
				}
				else {
					ptr++;
				}
			}
			*/
		}
	}
	
	closesocket(s);
	WSACleanup();

	system("pause");
}