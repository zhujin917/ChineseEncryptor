#define VERSION "v0.5.1（内部预览版本）"
#define INTRODUCTION "大概是世界上第一个利用汉字进行文件加密的算法。"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <map>
#include <bitset>
#include <fstream>

using namespace std;

map<int, string> dic;
map<string, int> dicDic;

int bin2dec(vector<bool> binBits) {
	int result = 0;
	for (int i = 0; i < binBits.size(); i += 1) {
		if (binBits[binBits.size() - 1 - i]) {
			result += pow(2, i);
		}
	}
	return result;
}

vector<bool> dec2bin(int decNumber, int maxLength) {
	vector<bool> result;
	int num = decNumber;
	for (int i = 0; num != 0; i++) {
		result.push_back(num % 2);
		num /= 2;
	}
	result.resize(maxLength, 0);
	reverse(result.begin(), result.end());
	return result;
}

double seededRandom(long long int seed) {
	double random = ((seed * 9301 + 49297) % 233280) / double(233280);
	return round(random * pow(10, 8)) / pow(10, 8);
}

void init(string keyString) {
	int range1 = 0xf7 - 0xb0;
	int range2 = 0xfe - 0xa1;
	char code1, code2;
	char ch[3] = { 0 };
	string chs;

	for (int i = 0; i < 4098; i += 1) {
		if (i == 1672) {
			code1 = 0xe3;
			code2 = 0xa7;
		}
		else if (i == 2879) {
			code1 = 0xe4;
			code2 = 0xa8;
		}
		else {
			code1 = i % range1 + 0xb0;
			code2 = i % range2 + 0xa1;
		}
		ch[0] = code1;
		ch[1] = code2;
		chs = string(ch);
		dic[i] = chs;
		dicDic[chs] = i;
	}

	if (keyString.length() > 0) {
		vector<double> randomNum(keyString.length(), NULL);
		for (int i = dic.size() - 1; i >= 0; i -= 1) {
			int keyIndex = i % keyString.length();
			int seed = (randomNum[keyIndex] != NULL) ? round(randomNum[keyIndex] * pow(10, 8)) : int(keyString[keyIndex]);
			randomNum[keyIndex] = seededRandom(seed);

			int target = floor(randomNum[keyIndex] * (i + 1));
			swap(dic[i], dic[target]);

			dicDic[dic[i]] = i;
			dicDic[dic[target]] = target;
		}
	}
}

void die(string message, int exitCode) {
	if (exitCode == 0) {
		cout << message << endl;
	}
	else {
		cout << "\033[31;1m" << message << "\033[0m" << endl;
	}
	cout << "作者主页：https://www.jiuxiaoqi.top/" << endl;
	exit(exitCode);
}

vector<bool> readFileAsBits(string filePath) {
	ifstream file(filePath, ios::in | ios::binary);
	if (!file.is_open()) {
		die("无法打开文件：" + filePath, 2001);
	}
	file.seekg(0, ios::end);
	size_t fileLength = file.tellg();
	unsigned char* fileData = new unsigned char[fileLength]();
	file.seekg(0, ios::beg);
	file.read(reinterpret_cast<char*>(fileData), sizeof(char) * fileLength);
	file.close();

	vector<bool> result;
	int refreshLength = floor(fileLength / 100);
	for (size_t i = 0; i < fileLength; i += 1) {
		bitset<8> bits;
		bits = fileData[i];
		for (size_t j = 0; j < 8; j += 1) {
			result.push_back(bits[8 - j - 1]);
		}

		if (i % refreshLength == 0) {
			cout << "\r读取文件 已完成 " << floor(100 * i / fileLength) << "%";
		}
	}
	cout << "\r读取文件 已完成 100%" << endl;
	return result;
}

void writeFileWithString(string filePath, string fileContent) {
	ofstream file(filePath, ios::out | ios::binary);
	if (!file.is_open()) {
		die("无法打开文件：" + filePath, 2002);
	}
	file << fileContent;
	file.close();
	cout << "\r写入文件 已完成 100%" << endl;
}

string readFileAsString(string filePath) {
	ifstream file(filePath, ios::in);
	if (!file.is_open()) {
		die("无法打开文件：" + filePath, 2003);
	}
	string fileData;
	file >> fileData;
	file.close();
	cout << "\r读取文件 已完成 100%" << endl;
	return fileData;
}

void writeFileWithBits(string filePath, vector<bool> fileContent) {
	ofstream file(filePath, ios::out | ios::binary);
	if (!file.is_open()) {
		die("无法打开文件：" + filePath, 2004);
	}

	int refreshLength = floor(fileContent.size() / 800) * 8;
	for (int i = 0; i < fileContent.size(); i += 8) {
		vector<bool> eightBits(8);
		vector<bool>::iterator first = fileContent.begin() + i;
		vector<bool>::iterator last = fileContent.begin() + i + 8;
		eightBits.assign(first, last);
		file << char(bin2dec(eightBits));

		if (i % refreshLength == 0) {
			cout << "\r写入文件 已完成 " << floor(100 * i / fileContent.size()) << "%";
		}
	}
	cout << "\r写入文件 已完成 100%" << endl;
	file.close();
}

string encode(vector<bool> sourceBits) {
	string result = "";
	int refreshLength = floor(sourceBits.size() / 1200) * 12;
	for (int i = 0; i < sourceBits.size(); i += 12) {
		vector<bool> twelveBits(12);
		vector<bool>::iterator first = sourceBits.begin() + i;
		vector<bool>::iterator last = (i + 12 < sourceBits.size()) ? (sourceBits.begin() + i + 12) : sourceBits.end();
		twelveBits.assign(first, last);
		twelveBits.resize(12, 0);

		int index = bin2dec(twelveBits);
		result += dic[index];
		for (int j = 0; j < floor((12 - (last - first)) / 8); j += 1) {
			result += dic[4096];
		}

		if (i % refreshLength == 0) {
			cout << "\r加密　　 已完成 " << floor(100 * i / sourceBits.size()) << "%";
		}
	}
	while ((result.length() * 12 / 2) % 8 != 0) {
		result += dic[4097];
	}
	cout << "\r加密　　 已完成 100%" << endl;
	return result;
}

vector<bool> decode(string encodedString) {
	vector<bool> result;
	int refreshLength = floor(encodedString.size() / 200) * 2;
	for (int i = 0; i < encodedString.size(); i += 2) {
		string chr = encodedString.substr(i, 2);
		int index = dicDic[chr];
		if (index == 4096) {
			result.erase(result.end() - 8, result.end());
			continue;
		}
		else if (index == 4097) {
			break;
		}
		vector<bool> twelveBits(12);
		twelveBits = dec2bin(index, 12);
		result.insert(result.end(), twelveBits.begin(), twelveBits.end());

		if (i % refreshLength == 0) {
			cout << "\r解密　　 已完成 " << floor(100 * i / encodedString.size()) << "%";
		}
	}
	result.resize(floor(result.size() / 8) * 8);
	cout << "\r解密　　 已完成 100%" << endl;
	return result;
}

int main(int argc, char* argv[]) {
	cout << "Chinese Encryptor " << VERSION << endl;
	cout << INTRODUCTION << endl;
	cout << "====================================================" << endl;

	string taskName, inputPath, outputPath, key;
	if (argc == 1) {
		cout << "参数 1：encrypt（加密）/ decrypt（解密）；" << endl;
		cout << "参数 2：已存在文件的绝对路径；" << endl;
		cout << "参数 3：生成文件的绝对路径；" << endl;
		cout << "（可选）参数 4：使用的密钥字符串。" << endl;
		exit(0);
	}
	else if (argc == 4 || argc == 5) {
		taskName = argv[1];
		inputPath = argv[2];
		outputPath = argv[3];
		key = "";
		if (argc == 5) {
			key = argv[4];
		}
	}
	else {
		die("参数错误。", 1001);
	}

	cout << "初始化　 进行中";
	init(key);
	cout << "\r初始化　 已完成" << endl;

	if (taskName == "encrypt") {
		writeFileWithString(outputPath, encode(readFileAsBits(inputPath)));
	}
	else if (taskName == "decrypt") {
		writeFileWithBits(outputPath, decode(readFileAsString(inputPath)));
	}
	else {
		die("参数错误。", 1002);
	}

	die("已顺利完成所有操作。", 0);
}
