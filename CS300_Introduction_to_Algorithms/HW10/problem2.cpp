#include <iostream>
#include <fstream>
#include <stack>
using namespace std;

void editSequence(string& a, string& b){
	ofstream output;
	output.open("output.txt");
	int m = (int)a.length();
	int n = (int)b.length();

	int dp[m+1][n+1];
	char bp[m+1][n+1];
	dp[0][0]=0;
	bp[0][0]='x';
	for(int i=1;i<=m;i++){
		dp[i][0]=i;
		bp[i][0]=a[i-1];
	}
	for(int i=1;i<=n;i++){
		dp[0][i]=i;
		bp[0][i]=b[i-1];
	}
	for(int j=1;j<=n;j++){
		for(int i=1;i<=m;i++){
			if(a[i-1]==b[j-1]){
				dp[i][j]=dp[i-1][j-1];
				bp[i][j]='=';
			}
			else{
				dp[i][j]=min(dp[i-1][j-1]+1,min(dp[i][j-1]+1,dp[i-1][j]+1));
				bool del=false;
				bool rep=false;
				bool ins=false;
				if(dp[i][j]==dp[i-1][j-1]+1){ //replace is 대각선 아래오른쪽
					rep=true;
				}
				if(dp[i][j]==dp[i][j-1]+1){   //insert is 아래쪽
					ins=true;
				}
				if(dp[i][j]==dp[i-1][j]+1){   //delete is 오른쪽
					del=true;
				}
				if(rep&&del&&ins){   //replace
					bp[i][j]='a';
				}else if(rep&&del){  //replace
					bp[i][j]='j';
				}else if(rep&&ins){  //replace
					bp[i][j]='k';
				}else if(del&&ins){  //maybe delete
					bp[i][j]='l';
				}else if(rep){       //replace
					bp[i][j]='r';
				}else if(ins){       //insert
					bp[i][j]='i';
				}else if(del){       //delete
					bp[i][j]='d';
				}
			}
		}
	}
	cout << "--------------" << endl;
	cout << "the edit table" << endl;
	cout << "--------------" << endl;
	for(int j=0;j<=n;j++){
		for(int i=0;i<=m;i++){
			cout << dp[i][j] << "\t";
		}
		cout << endl;
	}
	int i = m;
	int j = n;
	char op;
	bool print=true;
	stack<string> s;
	cout << "---------" << endl;
	cout << "procedure" << endl;
	cout << "---------" << endl;
	while(true){
		if(print){
			s.push(b);
		}else{
			print=true;
		}
		if(b==a){
			break;
		}
		op=bp[i][j];
		if(op=='r'||op=='a'||op=='j'||op=='k'){
			b[j-1]=a[i-1];
			i--;
			j--;
		}else if(op=='i'){
			b.erase((unsigned long)j);
			j--;
		}else if(op=='d'){
			b.insert((unsigned long)j,1,a[i-1]);
			i--;
		}else if(op=='='){
			i--;
			j--;
			print=false;
		}else if(op=='l'){
			b.insert((unsigned long)j,1,a[i-1]);
			i--;
		}
	}
	while(!s.empty()){
		cout << s.top() << endl;
		output << s.top();
		s.pop();
		if(!s.empty()){
			output << endl;
		}
	}
	output.close();
}

int main() {
	fstream T1;
	fstream T2;
	T1.open("raw1.txt");  //abcde
	T2.open("raw2.txt");  //acef
	string a,b;
	getline(T1,a);
	getline(T2,b);
	T1.close();
	T2.close();
	editSequence(a,b);
	return 0;
}