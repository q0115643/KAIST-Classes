#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class node{
public:
	string data;
	struct node *left;
	struct node *right;
	struct node *parent;
	node() {
		left = NULL;
		right = NULL;
		parent = NULL;
	}
};


class Qnode{
public:
	node *key;
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
	void enQueue(node* n){
		Qnode *temp = new Qnode;
		temp->key = n;
		temp->next = NULL;
		if(front==NULL){
			front = temp;
		}else{
			rear->next = temp;
		}
		rear = temp;
		size++;
	}
	node* deQueue(){
		Qnode *temp = new Qnode;
		if(front==NULL){
			cout << "Queue is empty" << endl;
		}
		else{
			temp = front;
			front = front->next;
			node* result = temp->key;
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


class BinTree {
public:
	node *root;
	node *current;

	BinTree() {
		root = NULL;
		current = NULL;
	}

	bool IsEmpty() {
		if (root == NULL) {
			return true;
		} else {
			return false;
		}
	}

	BinTree Lchild() {
		if (root == NULL) {
			exit(1);
		}
		BinTree *Lsubtree = new BinTree();
		Lsubtree->root = root->left;
		return *Lsubtree;
	}

	BinTree Rchild() {
		if (root == NULL) {
			exit(2);
		}
		BinTree *Rsubtree = new BinTree();
		Rsubtree->root = root->right;
		return *Rsubtree;
	}
};

void BFSprint(node* root){
	Queue Q = Queue();
	if(root){
		Q.enQueue(root);
	}
	while(!Q.empty()){
		int i = Q.size;
		while(i>0) {
			node *N = Q.deQueue();
			cout << N->data << " ";
			if (N->left) {
				Q.enQueue(N->left);
			}
			if (N->right) {
				Q.enQueue(N->right);
			}
			i--;
		}
		cout << endl;
	}
}


void store(node* root){
	ofstream output;
	output.open("data.txt");
	Queue Q = Queue();
	if(root){
		Q.enQueue(root);
	}
	bool stop = false;
	while(!Q.empty()&&!stop){
		stop = true;
		int i = Q.size;
		while(i>0) {
			node *N = Q.deQueue();
			output << N->data;
			if(i>1){
				output << " ";
			}
			if (N->left) {
				Q.enQueue(N->left);
				stop=false;
			}
			else{
				node *dot = new node();
				dot->data = ".";
				Q.enQueue(dot);
			}
			if (N->right) {
				Q.enQueue(N->right);
				stop=false;
			}
			else{
				node *dot = new node();
				dot->data = ".";
				Q.enQueue(dot);
			}
			i--;
		}
		if(!stop) {
			output << endl;
		}
	}
}

int countWords(string s)
{
	int number = 1;
	for(int i = 0; i < s.length();i++) {
		if (s[i] == ' ') {
			number++;
		}
	}
	return number;
}

BinTree* buildTree(){

	ifstream d;
	d.open("data.txt");
	string s;
	int numberoflines=0;
	while (getline(d, s)) {
		++numberoflines;
	}
	d.close();

	ifstream input;
	input.open("data.txt");
	string line;
	string* lines = new string[numberoflines];

	BinTree* newtree = new BinTree();

	for (int i=0;i<numberoflines;i++){
		getline(input, line);
		lines[i] = line;
	}
	input.close();
	int* N = new int[numberoflines];
	for (int i=0;i<numberoflines;i++){
		N[i] = countWords(lines[i]);
	}
	node** nodes = new node*[500];
	node** nodes1 = new node*[500];
	node *Root = new node();
	newtree->root=Root;
	for (int i=0;i<numberoflines;i++){
		if (i==0){
			Root->data = lines[0];
			nodes[0]=Root;
		}
		else{
			string word;
			istringstream iss(lines[i]);
			int k = 0;
			while(iss>>word){
				node *newnode = new node();
				newnode->data = word;
				nodes1[k]=newnode;
				if(word!="."){
					if(k%2==0){
						nodes[k/2]->left=newnode;
					}else{
						nodes[k/2]->right=newnode;
					}
				}
				k++;
			}
			for(int l=0;l<500;l++){
				nodes[l]=nodes1[l];
			}
		}
	}
	return newtree;
}

int main() {
	BinTree* T = buildTree();
	BFSprint(T->root);
	store(T->root);
	//to make it doesn't close in exe file
	char a;
	cin >> a;
	return 0;
}