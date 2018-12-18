#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
using namespace std;

void split(const string &s, const char* delim, vector<string> &v){
	char * dup = strdup(s.c_str());
	char * token = strtok(dup, delim);
	while(token != NULL){
		v.push_back(string(token));
		token = strtok(NULL, delim);
	}
	free(dup);
}

int hextoi(string hex)
{
	int num = 0;
	int pow16 = 1;
	string alpha("0123456789ABCDEF");
	if(hex[1] == 'x') {
		for (int i = hex.length() - 1; i >= 2; --i) {
			num += alpha.find(toupper(hex[i])) * pow16;
			pow16 *= 16;
		}
	}
	else {
		for (int i = hex.length() - 1; i >= 0; --i) {
			num += alpha.find(toupper(hex[i])) * pow16;
			pow16 *= 16;
		}
	}
	return num;
}

int stoDec(string str) {
	if (str[0]=='0' && str[1]=='x') {
		// 16진수 hexadecimal
		return hextoi(str);
	}
	else {
		return stoi(str);
	}
}

string itoBinary(int num) {
	string bin = "";
	int mask = 1;
	for(int i = 0; i < 32; i++)
	{
		string one = "1";
		string zero = "0";
		if((mask & num) != 0) {
			bin = one.append(bin);
		}
		else {
			bin = zero.append(bin);
		}
		mask<<=1;
	}
	return bin;
}

string hextoBinary(string hex) {
	return itoBinary(hextoi(hex));
}

string hexto6bitsBin(string hex) {
	return hextoBinary(hex).substr(26,6);
}

string resisterFormat(string resister) {
	string result;
	if(resister[0]=='$') {
		resister.erase(resister.begin());
		result = itoBinary(stoi(resister)).substr(27,5);
	}
	return result;
}

string Rformat(string opcode, string funct, string rd, string rs, string rt, string sa) {
	string result = "";
	result.append(opcode);
	result.append(resisterFormat(rs));
	result.append(resisterFormat(rt));
	result.append(resisterFormat(rd));
	result.append(sa);
	result.append(funct);
	return result;
}

string Iformat(string opcode, string rs, string rt, int imm) {
	string result = "";
	result.append(opcode);
	result.append(resisterFormat(rs));
	result.append(resisterFormat(rt));
	result.append(itoBinary(imm).substr(16,16));
	return result;
}

string Jformat(string opcode, int target) {
	string result = "";
	result.append(opcode);
	string secondElement = itoBinary(target).substr(6, 26);
	result.append(secondElement);
	return result;
}

int main(int argc, char* argv[]){

	bool readData = false;
	bool readText = false;


	if (argc != 2) {
		cout << "Wrong input" << endl;
		return -1;
	}


	string inputFileName = argv[1];
	string line;
	ifstream inputFileFirst (inputFileName);

	string result = "";
	string values = "";
	string instructions = "";
	int valueCount = 0;
	int instructionCount = 0;

	// 데이터 이름 리스트
	vector<string> dataNames;
	// 데이터 리스트
	vector<int> dataList;
	// 데이터 이름당 데이터 갯수
	vector<int> wordsInData;
	// instruction box 이름 리스트
	vector<string> blockNames;
	// box당 instruction 갯수
	vector<int> linesInBlock;

	while ( getline (inputFileFirst, line) ) {
		vector<string> lineSplitted;
		split(line, "\t, ", lineSplitted);

		if (lineSplitted[0]==".data") {
			readData = true;
			readText = false;
		}
		else if (lineSplitted[0]==".text") {
			readData = false;
			readText = true;
		}
		else if (readData) {
			// data 이름으로 시작하는 경우
			if (lineSplitted[0][lineSplitted[0].length()-1]==':') {
				// : 떼기
				lineSplitted[0].pop_back();
				// 데이터 이름
				string dataName = lineSplitted[0];
				// 데이터 이름 리스트에 푸시
				dataNames.push_back(dataName);
				// 이름당 데이터 개수 설정
				wordsInData.push_back(0);
				// 두 번째 단어는 .word (항상이지만 implement)
				if (lineSplitted[1]==".word") {
					// string을 십진수로 바꾸기
					int data = stoDec(lineSplitted[2]);
					dataList.push_back(data);
					wordsInData.back()++;
					string wordBin = itoBinary(data);
					values.append(wordBin);
					valueCount++;
				}
			}
				// .word로 시작인 경우
			else if (lineSplitted[0]==".word") {
				int data = stoDec(lineSplitted[1]);
				dataList.push_back(data);
				wordsInData.back()++;
				string wordBin = itoBinary(data);
				values.append(wordBin);
				valueCount++;
			}
		}
		else if (readText) {
			// block일때 (~:)
			if (lineSplitted[0][lineSplitted[0].length()-1]==':') {
				lineSplitted[0].pop_back();
				string blockName = lineSplitted[0];
				blockNames.push_back(blockName);
				linesInBlock.push_back(0);
			}
			// operations
			else {
				string operation = lineSplitted[0];
				instructionCount++;
				linesInBlock.back()++;
				if (operation == "la") {
					string label = lineSplitted[2];
					int address = hextoi("0x10000000");
					// label의 주소 찾기
					// count: 해당 label이 저장되기 전 words의 갯수
					int count = 0;
					int i = 0;
					while (dataNames[i] != label) {
						count += wordsInData[i];
						i++;
					}
					address += count*4;
					int underMask = hextoi("0x0000ffff");
					int underAddress = address&underMask;
					if (underAddress != 0) {
						linesInBlock.back()++;
						instructionCount++;
					}
				}
			}
		}
	}
	inputFileFirst.close();

	// 다시 열어서 instruction의 branching과 jump
	int currentCount = 0;
	ifstream inputFile (inputFileName);
	while ( getline (inputFile, line) )
	{

		vector<string> lineSplitted;
		split(line, "\t, ()", lineSplitted);

		if (lineSplitted[0]==".data") {
			readData = true;
			readText = false;
		}
		else if (lineSplitted[0]==".text") {
			readData = false;
			readText = true;
		}
		// data를 읽을 때
		else if (readData) {
		}
		// text를 읽을 때
		else if (readText) {
			// block일때 (~:)
			if (lineSplitted[0][lineSplitted[0].length()-1]==':') {
			}
			// operations
			else {
				string operation = lineSplitted[0];
				currentCount++;
				if (operation == "and") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("24");
					string rd = lineSplitted[1];
					string rs = lineSplitted[2];
					string rt = lineSplitted[3];
					string sa = "00000";
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "la") {
					string rt = lineSplitted[1];
					string label = lineSplitted[2];
					int address = hextoi("0x10000000");
					// label의 주소 찾기
					// count: 해당 label이 저장되기 전 words의 갯수
					int count = 0;
					int i = 0;
					while (dataNames[i] != label) {
						count += wordsInData[i];
						i++;
					}
					address += count*4;
					int upperMask = hextoi("0xffff0000");
					int underMask = hextoi("0x0000ffff");
					int upperAddress = (address&upperMask) >> 16;
					int underAddress = address&underMask;
					// lui
					string luiOpcode = hexto6bitsBin("f");
					string rs = "$0";
					int luiImm = upperAddress;
					string luiInstruction = Iformat(luiOpcode, rs, rt, luiImm);
					cout << "lui" << " " << lineSplitted[1] << " " << upperAddress << endl;
					cout << luiInstruction << endl;
					instructions.append(luiInstruction);
					if (underAddress != 0) {
						string oriOpcode = hexto6bitsBin("d");
						rs = rt;
						int oriImm = underAddress;
						string oriInstruction = Iformat(oriOpcode, rs, rt, oriImm);
						cout << "ori" << " " << lineSplitted[1] << " " << lineSplitted[1] << " " << underAddress << endl;
						cout << oriInstruction << endl;
						instructions.append(oriInstruction);
						currentCount++;
					}
				}
				else if (operation == "addiu") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("9");
					int imm = stoDec(lineSplitted[3]);
					string rt = lineSplitted[1];
					string rs = lineSplitted[2];
					string instruction = Iformat(opcode, rs, rt, imm);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "or") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("25");
					string rd = lineSplitted[1];
					string rs = lineSplitted[2];
					string rt = lineSplitted[3];
					string sa = "00000";
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "bne") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("5");
					string rs = lineSplitted[1];
					string rt = lineSplitted[2];
					int offset = 0;
					int countDest = 0;
					int i = 0;
					string label = lineSplitted[3];
					while (blockNames[i] != label) {
						countDest += linesInBlock[i];
						i++;
					}
					offset = countDest - currentCount;
					string instruction = Iformat(opcode, rs, rt, offset);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "sll") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("0");
					string sa = itoBinary(stoDec(lineSplitted[3])).substr(27, 5);
					string rd = lineSplitted[1];
					string rt = lineSplitted[2];
					string rs = "$0";
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "srl") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("2");
					string sa = itoBinary(stoDec(lineSplitted[3])).substr(27, 5);
					string rd = lineSplitted[1];
					string rt = lineSplitted[2];
					string rs = "$0";
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "addu") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("21");
					string sa = "00000";
					string rd = lineSplitted[1];
					string rs = lineSplitted[2];
					string rt = lineSplitted[3];
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "nor") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("27");
					string sa = "00000";
					string rd = lineSplitted[1];
					string rs = lineSplitted[2];
					string rt = lineSplitted[3];
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "beq") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("4");
					string rs = lineSplitted[1];
					string rt = lineSplitted[2];
					int offset = 0;
					int countDest = 0;
					int i = 0;
					string label = lineSplitted[3];
					while (blockNames[i] != label) {
						countDest += linesInBlock[i];
						i++;
					}
					offset = countDest - currentCount;
					string instruction = Iformat(opcode, rs, rt, offset);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "j") {
					cout << operation << " " << lineSplitted[1] << endl;
					string opcode = hexto6bitsBin("2");
					string label = lineSplitted[1];
					int count = 0;
					int i = 0;
					while (blockNames[i] != label) {
						count += linesInBlock[i];
						i++;
					}
					int address = hextoi("0x400000");
					address += 4*count;
					int target = address/4;
					string instruction = Jformat(opcode, target);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "ori") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("d");
					string rt = lineSplitted[1];
					string rs = lineSplitted[2];
					int imm = stoDec(lineSplitted[3]);
					string instruction = Iformat(opcode, rs, rt, imm);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "subu") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("23");
					string rd = lineSplitted[1];
					string rs = lineSplitted[2];
					string rt = lineSplitted[3];
					string sa = "00000";
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "andi") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("c");
					string rt = lineSplitted[1];
					string rs = lineSplitted[2];
					int imm = stoDec(lineSplitted[3]);
					string instruction = Iformat(opcode, rs, rt, imm);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "lui") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << endl;
					string opcode = hexto6bitsBin("f");
					string rs = "$0";
					string rt = lineSplitted[1];
					int imm = stoDec(lineSplitted[2]);
					string instruction = Iformat(opcode, rs, rt, imm);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "lw") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("23");
					string rt = lineSplitted[1];
					string rs = lineSplitted[3];
					int offset = stoDec(lineSplitted[2]);
					string instruction = Iformat(opcode, rs, rt, offset);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "sw") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("2b");
					string rt = lineSplitted[1];
					string rs = lineSplitted[3];
					int offset = stoDec(lineSplitted[2]);
					string instruction = Iformat(opcode, rs, rt, offset);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "jal") {
					cout << operation << " " << lineSplitted[1] << endl;
					string opcode = hexto6bitsBin("3");
					string label = lineSplitted[1];
					int count = 0;
					int i = 0;
					while (blockNames[i] != label) {
						count += linesInBlock[i];
						i++;
					}
					int address = hextoi("0x400000");
					address += 4*count;
					int target = address/4;
					string instruction = Jformat(opcode, target);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "jr") {
					cout << operation << " " << lineSplitted[1] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("8");
					string rt = "$0";
					string rd = "$0";
					string rs = lineSplitted[1];
					string sa = "00000";
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "sltiu") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("b");
					string rt = lineSplitted[1];
					string rs = lineSplitted[2];
					int imm = stoDec(lineSplitted[3]);
					string instruction = Iformat(opcode, rs, rt, imm);
					cout << instruction << endl;
					instructions.append(instruction);
				}
				else if (operation == "sltu") {
					cout << operation << " " << lineSplitted[1] << " " << lineSplitted[2] << " " << lineSplitted[3] << endl;
					string opcode = hexto6bitsBin("0");
					string funct = hexto6bitsBin("2b");
					string rd = lineSplitted[1];
					string rs = lineSplitted[2];
					string rt = lineSplitted[3];
					string sa = "00000";
					string instruction = Rformat(opcode, funct, rd, rs, rt, sa);
					cout << instruction << endl;
					instructions.append(instruction);
				}
			}
		}

	}
	inputFile.close();

	//쓰기~
	string valueCountString = itoBinary(valueCount*4);
	string instructionCountString = itoBinary(instructionCount*4);
	result.append(instructionCountString);
	result.append(valueCountString);
	result.append(instructions);
	result.append(values);


	string outputFileName = inputFileName;
	outputFileName[outputFileName.length()-1]='o';
	ofstream outputFile (outputFileName);
	outputFile << result;
	outputFile.close();

	return 0;
}

