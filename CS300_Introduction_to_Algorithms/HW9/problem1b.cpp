#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

void BruteForce(string pattern, string text){
	ofstream output;
	output.open("output.txt");
	int m = (int)pattern.length();
	int n = (int)text.length();
	bool mismatch = false;
	bool first=true;
	for(int i=0;i<n-m;i++){
		int j=0;
		while(!mismatch&&j<m){
			if(text[i+j]!=pattern[j]){
				mismatch = true;
			}
			j++;
		}
		if(!mismatch){
			if(first){
				first = false;
			}else{
				cout << " ";
				output << " ";
			}
			cout << i;
			output << i;
		}
		else{
			mismatch = false;
		}
	}
}

string readfromFile(string name){
	ifstream p;
	p.open(name);
	stringstream strStream;
	strStream << p.rdbuf();
	string text = strStream.str();
	p.close();
	return text;
}

int main() {
	string pattern = readfromFile("pattern.txt");
	string text = readfromFile("raw.txt");
	cout << pattern << endl;
	cout << text << endl;
	BruteForce(pattern, text);
	return 0;
}