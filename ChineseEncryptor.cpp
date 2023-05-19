constexpr auto APP_VERSION = "v0.4.2（内部预览版本）";
constexpr auto APP_INFO = "大概是世界上第一个利用汉字进行文件加密的算法。";
#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>
#include <math.h>
// #include <stdio.h>

using namespace std;

string dic = "";
map<string, int>dicDic;
string fileProc;
string filePath;
string fileOut;
string fileKey = "";

int bin2dec(long long int bin) {
	int dec = 0, times = 0, temp;
	while (bin > 0) {
		temp = int(bin % 10);
		bin /= 10;
		dec += int(temp * pow(2, times += 1));
	}
	return dec;
}

string dec2bin(int dec) {
	int temp = dec, count = 0;
	string bin = "000000000000";
	while (temp > 0) {
		if (temp % 2 == 1) {
			bin[12 - count - 1] = '1';
		}
		temp /= 2;
		count += 1;
	}
	return bin;
}

string toUpper(string str) {
	string res = str;
	for (int i = 0; i < res.length(); i += 1) {
		if (res[i] >= 'a' && res[i] <= 'z') {
			res[i] -= 'a' - 'A';
		}
	}
	return res;
}

bool checkKey(string key) {
	if (key.length() != 64) {
		return false;
	}
	for (int i = 0; i < 64; i += 1) {
		if (!((key[i] >= 'A' && key[i] <= 'Z') || (key[i] >= '0' && key[i] <= '9'))) {
			return false;
		}
	}
	return true;
}

void init() {
	int range1 = 0xf7 - 0xb0;
	int range2 = 0xfe - 0xa1;
	char code1, code2;
	char ch[3] = { 0 };
	string chs;
	for (int i = 0; i < 4096; i += 1) {
		code1 = i % range1 + 0xb0;
		code2 = i % range2 + 0xa1;
		ch[0] = code1;
		ch[1] = code2;
		chs = string(ch);
		dic += chs;
		dicDic[chs] = i;
	}
	if (fileKey.length() > 0) {
		int key = 0, toKey = 0;
		for (int i = 0; i < 4096; i += 1) {
			key = i % 64;
			if (fileKey[key] >= '0' && fileKey[key] <= '9') {
				toKey = (i * 2 + (fileKey[key] - '0') * 2) % 4096;
			}
			else if (fileKey[key] >= 'A' && fileKey[key] <= 'Z') {
				toKey = (i * 2 + (fileKey[key] - 'A' + 10) * 2) % 4096;
			}
			swap(dic[i * 2], dic[toKey]);
			swap(dic[i * 2 + 1], dic[toKey + 1]);

			ch[0] = dic[i * 2];
			ch[1] = dic[i * 2 + 1];
			chs = string(ch);
			dicDic[chs] = i;

			ch[0] = dic[toKey];
			ch[1] = dic[toKey + 1];
			chs = string(ch);
			dicDic[chs] = toKey / 2;
		}
	}
}

void encode() {
	cout << "正在读取文件..." << endl;
	ifstream file;
	file.open(filePath, ifstream::binary);
	if (!file) {
		cout << "无法读取文件。" << endl;
		return;
	}
	file.seekg(0, ios::end);
	size_t fileLength = file.tellg();
	unsigned char* fileData = new unsigned char[fileLength]();
	file.seekg(0, ios::beg);
	if (!file.eof()) {
		file.read(reinterpret_cast<char *>(fileData), sizeof(char) * fileLength);
	}
	file.close();
	vector<bool>fileBit;
	bitset<8>bit;
	for (size_t i = 0; i < fileLength; i += 1) {
		bit = fileData[i];
		for (size_t j = 0; j < 8; j += 1) {
			fileBit.push_back(bit[8 - j - 1]);
		}
		if ((i + 1) % 1728000 == 0) {
			cout << "读取文件 已完成 " << i + 1 << " / " << fileLength << endl;
		}
	}
	if (fileLength % 1728000 != 0) {
		cout << "读取文件 已完成 " << fileLength << " / " << fileLength << endl;
	}
	cout << "正在加密..." << endl;
	int fileChar = 0;
	long long int fileTemp = 0;
	string ret = "";
	for (size_t i = 0; i < fileBit.size(); i += 1) {
		fileTemp *= 10;
		fileTemp += int(fileBit[i]);
		if ((i + 1) % 12 == 0) {
			fileChar = bin2dec(fileTemp);
			ret += dic[fileChar];
			ret += dic[fileChar + 1];
			fileTemp = 0;
			if ((i + 1) % 5959000 == 0) {
				cout << "加密 已完成 " << i + 1 << " / " << fileBit.size() << endl;
			}
		}
	}
	if (fileBit.size() % 5959000 != 0) {
		cout << "加密 已完成 " << fileBit.size() << " / " << fileBit.size() << endl;
	}
	ofstream fout(fileOut, ios::out);
	fout << ret;
	fout.close();
}

void decode() {
	cout << "正在读取文件..." << endl;
	ifstream file;
	file.open(filePath, ifstream::in);
	if (!file) {
		cout << "无法读取文件。" << endl;
		return;
	}
	string fileData;
	file >> fileData;
	string fileCharBin = "";
	vector<bool>fileBit;
	for (size_t i = 0; i < fileData.size(); i += 2) {
		fileCharBin = dec2bin(dicDic[string(1, fileData[i]) + string(1, fileData[i + 1])]);
		for (size_t j = 0; j < 12; j += 1) {
			if (fileCharBin[j] == '0') {
				fileBit.push_back(0);
			}
			else {
				fileBit.push_back(1);
			}
		}
		if ((i + 1) % 172800 == 0) {
			cout << "读取文件 已完成 " << i + 1 << " / " << fileData.size() << endl;
		}
	}
	if (fileData.size() % 172800 != 0) {
		cout << "读取文件 已完成 " << fileData.size() << " / " << fileData.size() << endl;
	}
	cout << "正在解密..." << endl;
	string ret = "";
	int fileTemp = 0;
	for (size_t i = 0; i < fileBit.size(); i += 1) {
		fileTemp *= 10;
		fileTemp += fileBit[i];
		if ((i + 1) % 8 == 0) {
			ret += char(bin2dec(fileTemp) / 2);
			fileTemp = 0;
		}
		if ((i + 1) % 5959000 == 0) {
			cout << "解密 已完成 " << i + 1 << " / " << fileBit.size() << endl;
		}
	}
	if (fileBit.size() % 5959000 != 0) {
		cout << "解密 已完成 " << fileBit.size() << " / " << fileBit.size() << endl;
	}
	if (fileBit.size() % 8 != 0) {
		ret += char(bin2dec(fileTemp) / 2);
		// fileTemp = 0;
	}
	ofstream fout(fileOut, ifstream::binary | ios::out);
	fout << ret;
	fout.close();
}

int main(int argc, char *argv[]) {
	cout << "Chinese Encryptor " << APP_VERSION << endl;
	cout << APP_INFO << endl;
	cout << "====================================================" << endl;

	if (argc > 1) {
		fileProc = argv[1];
	}
	if (argc == 1 || fileProc == "?") {
		cout << "参数 1：encrypt（加密）/ decrypt（解密）；" << endl;
		cout << "参数 2：已存在文件的绝对路径；" << endl;
		cout << "参数 3：生成文件的绝对路径；" << endl;
		cout << "（可选）参数 4：使用的密钥字符串。" << endl;
		return 0;
	}

	cout << "正在初始化..." << endl;
	filePath = argv[2];
	fileOut = argv[3];
	if (argc >= 5) {
		fileKey = toUpper(argv[4]);
		if (!checkKey(fileKey)) {
			cout << "密钥字符串参数错误。" << endl;
			return 0;
		}
	}
	init();

	if (fileProc == "encrypt") {
		encode();
	}
	else if (fileProc == "decrypt") {
		decode();
	}
	else {
		cout << "参数错误。" << endl;
		return 0;
	}
	cout << "所有处理已完成。" << endl;
	cout << "作者主页：https://ray917.top/" << endl;

	return 0;
}
