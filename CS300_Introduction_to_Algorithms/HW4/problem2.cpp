#include <iostream>
#include <fstream>
#include <queue>

#define MAX_TOK 255
using namespace std;

/*
 *  Adjacency-Matrix
 */
class adjMatrix{
private:
	int n;
public:
	int **cell; //2 dimensional array of size n*n as the adjacency-matrix
	adjMatrix(int n){
		this->n = n;
		cell = new int*[n];
		for (int i=0;i<n;i++){
			cell[i]=new int[n];
			for (int j=0;j<n;j++){
				cell[i][j]=0;       //set every cells to 0
			}
		}
	}
	void addEdgeMdir(int from, int to){      //if it's directed graph, add just one edge from 'from' to 'to'
		cell[from-1][to-1]=1;
	}
	void addEdgeMundir(int from, int to){    //if it's undirected, add 'from' to 'to' and 'to' to 'from'
		cell[from-1][to-1]=1;
		cell[to-1][from-1]=1;
	}
	bool existPath(int from, int to){         //return true if path from 'from' to 'to' exists
		from--;
		to--;
		if (from == to){
			return true;
		}
		bool *visited = new bool[n];          //mark of each node that is visited or not
		for (int i = 0; i < n; i++) {
			visited[i] = false;               //set it all to false
		}
		queue <int> Que;                      //make a queue
		Que.push(from);                       //just follow the steps of BFS to check all nodes that can be visited
		visited[from]=true;
		while(!Que.empty()){
			from = Que.front();
			Que.pop();
			for(int i=0;i<n;i++){
				if(cell[from][i]==1){
					if(i==to){
						return true;
					}
					if(!visited[i]){
						visited[i]=true;
						Que.push(i);
					}
				}
			}
		}
		return false;
	}
};

/*
 *   Adjacency-List
 */
struct adjListNode{                      //node of adj-list
	int dest;                            //destination from the flag
	struct adjListNode* next;            //pointer to the next node
};
struct adjList                           //adj-list of one dimension (adj[i] of vertex i)
{
	struct adjListNode* flag;            //point to the flag of list (on the last of the list)
};
class AdjList
{
private:
	int n;
	struct adjList* adjlst;
public:
	AdjList(int n)
	{
		this->n = n;
		adjlst=new adjList[n];
		for (int i=0;i<n;i++) {
			adjlst[i].flag = NULL;
		}
	}
	adjListNode* newadjNode(int dest)
	{
		adjListNode* newNode = new adjListNode;
		newNode->dest = dest;
		newNode->next = NULL;
		return newNode;
	}
	void addEdgeLundir(int from, int to)          //add edges in case of undirected adj-list
	{
		adjListNode* newNode = newadjNode(to);    //save new node which have 'to' to the adjlst[from]
		newNode->next = adjlst[from].flag;
		adjlst[from].flag = newNode;
		newNode = newadjNode(from);               //save new node which have 'from' to the adjlst[to]
		newNode->next = adjlst[to].flag;
		adjlst[to].flag = newNode;
	}
	void addEdgeLdir(int from, int to)            //add edges in case of directed adj-list
	{
		adjListNode* newNode = newadjNode(to);    //save new node which have 'to' to the adjlst[from]
		newNode->next = adjlst[from].flag;
		adjlst[from].flag = newNode;
	}
};

string* strSplit(string strOrigin, string strTok) {     //from input string strOrigin, split it by strTok
	int cutAt;
	int index = 0;
	string* strResult = new string[MAX_TOK];        // result goes in  strResult

	while ( (cutAt = (int)strOrigin.find_first_of(strTok)) != strOrigin.npos){
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

int main() {

	string line;
	string *Q = new string[3];  //strings where the Question a b c goes in

	//put "\n" at the end of the file to use getline til the end of the file.
	std::ofstream out;
	out.open("input.txt", std::ios::app);
	std::string enter = "\n";
	out << enter;
	out.close();

	ofstream writeFile("output.txt");        //output.txt

	ifstream openFile("input.txt");          //read input.txt
	getline(openFile, line);

	int numberoftest = (int)line[0]-48;    //read number of tests

	for (int i = 0; i < numberoftest; i++) {       //execute the procedure (# of tests) times
		getline(openFile, line);
		string *str = strSplit(line, " ");
		int DorUnd = (int)str[0][0]-48;
		int n = (int)str[1][0]-48;
		string *lines = new string[n];
		for (int i = 0; i < n; i++) {           //read lines of input edges
			getline(openFile, line);
			lines[i] = line;
		}
		for (int i = 0; i < 3; i++) {
			getline(openFile, line);
			Q[i] = line;                         //Q[0] for Question a, Q[1] for b, Q[2] for c
		}
		if (DorUnd == 0) {                       //when undirected graph
			adjMatrix adjM(n);
			AdjList adjL(n);

			for (int i = 0; i < n; i++) {         //addEdges the Adjacent-list and Adjacent-matrix
				if (lines[i][0] != ';') {         //in line, first char is the starting node(from)
					int from = lines[i][0] - 48;  //and 3rd is the ending(end)
					int to = lines[i][2] - 48;
					adjM.addEdgeMundir(from, to);
					adjL.addEdgeLundir(from, to);
				}
			}

			//start writing
			//Question a
			string *s = strSplit(Q[0], " ");

			int numA = (int) s[1].length() / 4;            //# of cases in Question a
			writeFile << "(a) ";
			string *Qasplitcolon = strSplit(s[1], ";");    //split the line to get starting node and the ending node
			for (int i = 0; i < numA; i++) {
				int from = Qasplitcolon[i][0] - 48;
				int to = Qasplitcolon[i][2] - 48;
				if (adjM.cell[from - 1][to - 1] == 1) {    //if there's an edge from from to to, write "T"
					writeFile << "T;";
				} else {
					writeFile << "F;";
				}
			}
			//Question b
			writeFile << "\n";
			string *ss = strSplit(Q[1], " ");
			int numB = (int) ss[1].length() / 2;
			writeFile << "(b) ";
			for (int i = 0; i < numB; i++) {
				writeFile << i + 1;
				for (int j = 0; j < n; j++) {
					if (adjM.existPath(i+1, j+1)) {         //for writing a list of all vertices which have a path
						writeFile << ",";                   //from i, use existPath(i,j) I defined.
						writeFile << j + 1;
					}
				}
				writeFile << ";";
			}
			//Question c
			writeFile << "\n";
			string *sss = strSplit(Q[2], " ");
			int numC = (int) sss[1].length() / 4;
			writeFile << "(c) ";

			string *Qcsplitcolon = strSplit(sss[1], ";");
			for (int i = 0; i < numC; i++) {
				int from = Qcsplitcolon[i][0] - 48;
				int to = Qcsplitcolon[i][2] - 48;
				if (adjM.existPath(from, to)) {             //just use existPath(from,to) to check
					writeFile << "T;";
				} else {
					writeFile << "F;";
				}
			}
			writeFile << "\n";

			//now the question d!!!!!!!!!

			writeFile << "(d) ";
			int from = 0;
			int *visited = new int[n];       //mark of each node that is visited or not
			for (int i = 0; i < n; i++) {    //changed visited[n] to an array of int,
				visited[i] = 0;              //to check which we've wrote already(by 2)
			}

			bool done = false;
			while (!done) {                 //if it's time to stop, I'll change bool done to true
				while ((visited[from] == 1 || visited[from] == 2) && from < n) {
					from++;                //the first node that is not visited becomes 'from'
				}
				queue<int> Que;
				Que.push(from);            //by BFS check every node that has path from 'from'
				visited[from] = 1;
				while (!Que.empty()) {
					from = Que.front();
					Que.pop();
					for (int i = 0; i < n; i++) {
						if (adjM.cell[from][i] == 1) {
							if (visited[i] == 0) {
								visited[i] = 1;
								Que.push(i);
							}
						}
					}
				}
				bool comma = false;            //first input doesn't have comma
				for (int i = 0; i < n; i++) {
					if (visited[i] == 1) {
						if (comma) {
							writeFile << ",";
						}
						writeFile << i + 1;
						comma = true;
						visited[i] = 2;        //put all connected nodes with 'from' and mark it with 2
					}
				}
				writeFile << ";";
				done = true;
				for (int i = 0; i < n; i++) {   //when there's any unvisited node, repeat
					if (visited[i] == 0) {
						done = false;
					}
				}
			}
			writeFile << "\n";
			//end of question d!!!!!!!!!
		} else {
			//all except d is same in directed one becaus it's different in addedge.
			adjMatrix adjM(n);
			AdjList adjL(n);
			for (int i = 0; i < n; i++) {
				if (lines[i][0] != ';') {
					int from = lines[i][0] - 48;
					int to = lines[i][2] - 48;
					adjM.addEdgeMdir(from, to);
					adjL.addEdgeLdir(from, to);
				}
			}
			string *s = strSplit(Q[0], " ");
			int numA = (int) s[1].length() / 4;
			writeFile << "(a) ";
			string *Qasplitcolon = strSplit(s[1], ";");
			for (int i = 0; i < numA; i++) {
				int from = Qasplitcolon[i][0] - 48;
				int to = Qasplitcolon[i][2] - 48;
				if (adjM.cell[from - 1][to - 1] == 1) {
					writeFile << "T;";
				} else {
					writeFile << "F;";
				}
			}


			writeFile << "\n";
			string *ss = strSplit(Q[1], " ");
			int numB = (int) ss[1].length() / 2;
			writeFile << "(b) ";
			for (int i = 0; i < numB; i++) {
				writeFile << i + 1;
				for (int j = 0; j < n; j++) {
					if (adjM.cell[i][j] == 1) {
						writeFile << ",";
						writeFile << j + 1;
					}
				}
				writeFile << ";";
			}

			writeFile << "\n";
			string *sss = strSplit(Q[2], " ");
			int numC = (int) sss[1].length() / 4;
			writeFile << "(c) ";
			string *Qcsplitcolon = strSplit(sss[1], ";");
			for (int i = 0; i < numC; i++) {
				int from = Qcsplitcolon[i][0] - 48;
				int to = Qcsplitcolon[i][2] - 48;
				if (adjM.existPath(from, to)) {
					writeFile << "T;";
				} else {
					writeFile << "F;";
				}
			}
			writeFile << "\n(d) directed graph # in case of directed graph, don't solve problem d" << endl;
		}
	}
	openFile.close();
	writeFile.close();

	return 0;
}
