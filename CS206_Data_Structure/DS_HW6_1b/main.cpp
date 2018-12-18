#include <iostream>
using namespace std;

void execute();

struct AdjList{
	int node;
	struct AdjList* link;
};

class Qnode{
public:
	AdjList* v;
	struct Qnode *next;
};

class Queue{
private:
	Qnode *rear;
	Qnode *front;
public:
	int size;
	Queue(){
		rear = NULL;
		front = NULL;
		size = 0;
	}
	void enQueue(AdjList* v){
		Qnode *temp = new Qnode;
		temp->v= v;
		temp->next = NULL;
		if(front==NULL){
			front = temp;
		}else{
			rear->next = temp;
		}
		rear = temp;
		size++;
	}
	AdjList* deQueue(){
		Qnode *temp;
		if(front==NULL){
			cout << "Queue is empty" << endl;
		}
		else{
			temp = front;
			front = front->next;
			AdjList* result = temp->v;
			delete temp;
			size--;
			return result;
		}
	}
	bool empty(){
		if(front==NULL){
			return true;
		}
		else{
			return false;
		}
	}
};

class Graph{
private:
	int V;
	struct AdjList* adjL;
public:
	Graph(int V){
		this->V=V;
		adjL=new AdjList[V];
		for (int i=0;i<V;i++){
			adjL[i].link=NULL;
			adjL[i].node=i;
		}
	}
	void addEdge(int u, int v){
		AdjList* newnode = new AdjList;
		newnode->link=NULL;
		newnode->node=v;
		AdjList* current = &adjL[u];
		AdjList* prev = current;
		while(current){
			prev=current;
			current=current->link;
		}
		prev->link=newnode;

		AdjList* newnode2 = new AdjList;
		newnode2->link=NULL;
		newnode2->node=u;
		AdjList* current2 = &adjL[v];
		AdjList* prev2 = current2;
		while(current2){
			prev2=current2;
			current2=current2->link;
		}
		prev2->link=newnode2;
	}
	int getInputu(){
		int u;
		cin >> u;
		if(u<-1||u>=V){
			cout << "Input is out of range, enter again." << endl;
			getInputu();
		}else{
			return u;
		}
	}
	int getInputv(){
		int v;
		cin >> v;
		if(v<0||v>=V){
			cout << "Input is out of range, enter again." << endl;
			getInputv();
		}else{
			return v;
		}
	}
	void addEdges(){
		int u, v;
		cout << "add new edges (u,v), u,v are integers which 0<=u<V, 0<=v<V" << endl;
		while(true) {
			cout << "for new edge (u,v)" << endl;
			cout << "Enter u, Enter -1 to quit adding edges" << endl;
			u=getInputu();
			if(u==-1){
				break;
			}
			cout << "Enter v" << endl;
			v=getInputv();
			addEdge(u,v);
		}
	}
	void printGraph()
	{
		cout << "Adjacency list" << endl;
		for (int i = 0; i < V; ++i)
		{
			AdjList* current = &adjL[i];
			bool first=true;
			while (current)
			{
				if(!first) {
					cout << "-> ";
				}else{
					cout << "head:";
					first=false;
				}
				cout<<current->node;
				current = current->link;
			}
			cout << "-> null" << endl;
		}
	}
	void BFS(){
		cout << "BFS spanning tree (starting from vertex 0)" << endl;
		if(V!=0) {
			AdjList *s = &adjL[0];
			bool *visited = new bool[V];
			for (int i = 0; i < V; i++) {
				visited[i] = false;
			}
			Queue Q = Queue();
			visited[s->node] = true;
			Q.enQueue(s);
			while (!Q.empty()) {
				int cnt = Q.size;
				for (int i = 0; i < cnt; i++) {
					s = &adjL[Q.deQueue()->node];
					cout << s->node << " ";
					AdjList *current = s;
					while (current) {
						if (!visited[current->node]) {
							visited[current->node] = true;
							Q.enQueue(current);
						}
						current = current->link;
					}
				}
				cout << endl;
			}
		}
	}
	void DFS(){
		cout << "DFS spanning tree (starting from vertex 0)" << endl;
		if(V!=0) {
			bool *visited = new bool[V];
			for (int i = 0; i < V; i++) {
				visited[i] = false;
			}
			DFSDFS(&adjL[0], visited);
			cout << endl;
			bool connected = true;
			for (int i = 0; i < V; i++) {
				if (!visited[i]) {
					connected = false;
				}
			}
			if (!connected) {
				cout << "----------------------------" << endl;
				cout << "the input is not a connected graph, do it again" << endl;
				execute();
			}
		}
	}
	void DFSDFS(AdjList* s, bool* visited){
		visited[s->node]=true;
		cout << s->node <<" ";
		while(s->link){
			s=s->link;
			if(!visited[s->node]){
				DFSDFS(&adjL[s->node], visited);
				break;
			}
		}
	}
	void search(AdjList* s, bool* visited){
		visited[s->node]=true;
		cout << s->node <<" ";
		while(s->link){
			s=s->link;
			if(!visited[s->node]){
				search(&adjL[s->node], visited);
			}
		}
	}
	void ConnectedComponents(){
		cout << "the connected components(separated with enter)" << endl;
		bool *visited = new bool[V];
		for(int i=0;i<V;i++){
			visited[i]=false;
		}
		for(int i=0;i<V;i++){
			if(!visited[i]){
				search(&adjL[i], visited);
				cout << endl;
			}
		}
	}
};

int setSize(){
	int V;
	cout << "Enter the number of vertices V" << endl;
	cin >> V;
	return V;
}

void execute(){
	const int V = setSize();
	Graph* graph= new Graph(V);
	graph->addEdges();
	cout << "----------------------------" << endl;
	graph->printGraph();
	cout << "----------------------------" << endl;
	//graph->DFS();
	//cout << "----------------------------" << endl;
	graph->BFS();
	cout << "----------------------------" << endl;
	//graph->ConnectedComponents();
	//cout << "----------------------------" << endl;
}

int main() {
	execute();
	return 0;
}