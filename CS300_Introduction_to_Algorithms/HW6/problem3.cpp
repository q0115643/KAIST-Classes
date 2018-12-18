#include <iostream>
#include <fstream>
#include <limits>
#define MAX_TOK 255

using namespace std;

string* strSplit(string strOrigin, string strTok) {     //from input string strOrigin, split it by strTok
	int cutAt;
	int index = 0;
	string* strResult = new string[MAX_TOK];        // result goes in  strResult
	while ( (cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos){
		if (cutAt > 0){
			strResult[index++] = strOrigin.substr(0, cutAt);
		}
		strOrigin = strOrigin.substr(cutAt + 1);
	}
	if (strOrigin.length() > 0){
		strResult[index++] = strOrigin.substr(0, cutAt);
	}
	return strResult;
}
int count_commas(string s) {
	int count = 0;

	for (int i = 0; i < s.size(); i++)
		if (s[i] == ',') count++;

	return count;
}
int main() {
	string line;
	std::ofstream out;
	out.open("input.txt", std::ios::app);
	std::string enter = "\n";
	out << enter;
	out.close();

	ofstream writeFile("output.txt");
	ifstream openFile("input.txt");

	getline(openFile, line);

	int n = std::stoi(line);
	string *lines = new string[n];
	int inf = std::numeric_limits<int>::max();
	int M[n][n];
	for (int i=0; i<n;i++){
		for(int j=0; j<n;j++){
			if (i==j){
				M[i][j]=0;
			}
			else {
				M[i][j] = inf;
			}
		}
	}
	for (int i = 0; i < n; i++) {           //read lines of input edges
		getline(openFile, line);
		lines[i] = line;
	}
	int commas=0;
	int numP;
	for (int i = 0; i<n; i++) {
		commas = count_commas(lines[i]);
		numP = commas + 1;
		string *delcolon = strSplit(lines[i], ";");
		string *delcomma = strSplit(delcolon[0], ",");
		for (int j =0; j<numP; j++){
			string *ss = strSplit(delcomma[j], " ");
			int l;
			int r;
			l = std::stoi(ss[0]);
			r = std::stoi(ss[1]);
			M[i][l-1]=r;
		}
	}
	int d[n+1][n][n];
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			int tmp = M[i][j];
			d[0][i][j] = tmp;
		}
	}

	for(int k=0; k<n; k++){
		for(int i=0; i<n; i++) {
			for (int j = 0; j < n; j++) {
				if ((d[k][i][k]==inf)||(d[k][k][j]==inf)){
					d[k+1][i][j] = d[k][i][j];
				}
				else if (d[k][i][j] > (d[k][i][k] + d[k][k][j])) {
					d[k+1][i][j] = (d[k][i][k] + d[k][k][j]);
				}
				else {
					d[k+1][i][j] = d[k][i][j];
				}
			}
		}
	}
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			if (d[n][i][j]==inf) {
				writeFile << "inf";
			}
			else {
				writeFile << d[n][i][j];
			}
			if (j!=n-1){
				writeFile << " ";
			}
		}
		if (i != n-1) {
			writeFile << endl;
		}
	}

	openFile.close();
	writeFile.close();
	return 0;
}