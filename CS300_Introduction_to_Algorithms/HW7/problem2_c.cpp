#include <iostream>
#include <fstream>
using namespace std;

int count(int n, int m, int S[]) {
	int dp[n+1][m];
	for (int i=0;i<=n;i++){
		for (int j=0;j<m;j++){
			if(i==0){
				dp[i][j]=1;
			}
			else if(j==0){
				if(i%S[j]==0){
					dp[i][j]=1;
				}
				else{
					dp[i][j]=0;
				}
			}
			else if(S[j]>i){
				dp[i][j]=dp[i][j-1];
			}
			else{
				dp[i][j]=dp[i-S[j]][j]+dp[i][j-1];
			}
		}
	}
	return dp[n][m-1];
}

int main() {
	ifstream input;
	input.open("input.txt");
	int S[500];
	int number=0;
	string line;
	while(input>>S[number]){
		number++;
	}
	cout << number << endl;
	for (int i=0;i<number;i++){
		cout << S[i] << " ";
	}
	cout << endl;
	int result = count(S[number-1], number-1, S);
	cout << result;
	ofstream output;
	output.open("output.txt");
	output << result;
	input.close();
	output.close();
	return 0;
}