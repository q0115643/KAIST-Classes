#include <iostream>
using namespace std;

class node{
public:
	bool number;
	string data;
	struct node *left;
	struct node *right;
	struct node *parent;
	node() {
		left = NULL;
		right = NULL;
		parent = NULL;
		number = false;
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

double interp(node* root){
	Queue Q = Queue();
	if(root->data=="+"||root->data=="-"||root->data=="/"||root->data=="*"){
		string op = root->data;
		double left = interp(root->left);
		double right = interp(root->right);
		if(root->data=="+"){
			return left+right;
		}
		else if(root->data=="-"){
			return left-right;
		}
		else if(root->data=="*"){
			return left*right;
		}
		else if(right==0){
			cout << "error : division by zero" << endl;
			exit(1);
		}
		else{
			return left/right;
		}
	}
	else{
		string n = root->data;
		int result = stoi(n);
		return (double)result;
	}
}

BinTree* makeBT(BinTree* btL, BinTree* btR, node* item){
	BinTree *newTree = new BinTree();
	newTree->root = item;
	newTree->root->left = btL->root;
	btL->root->parent = newTree->root;
	newTree->root->right = btR->root;
	btR->root->parent = newTree->root;
	return newTree;
}

char* deleteSpace(string aexpr){
	int len = (int)aexpr.length();
	int cnt = 0;
	for (int i = 0; i<len; i++){
		if (aexpr[i]!=' '){
			cnt++;
		}
	}
	char* newExpr = new char[cnt];
	int i=0;
	int j=0;
	while (j<len){
		if(aexpr[j]!=' '){
			newExpr[i]=aexpr[j];
			i++;
			j++;
		}
		else{
			j++;
		}
	}
	return newExpr;
}

BinTree* makeTree(char* expr){
	if (isdigit(expr[0])){
		int num = atoi(expr);
		BinTree *newTree = new BinTree();
		node *NODE = new node();
		NODE->number=true;
		NODE->data=to_string(num);
		newTree->root = NODE;
		return newTree;
	}
	else{
		int len = (int)strlen(expr);
		int lenL = 0;
		int lenR = 0;
		char op;
		bool found = false;           //if the loop went to the middle operator +, -, *, /
		int i = 1;
		int depth = 0;
		while(!found){
			if(expr[i]=='('){
				depth++;
				i++;
				lenL++;
			}
			else if(expr[i]==')'){
				depth--;
				i++;
				lenL++;
			}
			else if((expr[i]=='+'||expr[i]=='-'||expr[i]=='/'||expr[i]=='*')&&depth==0){
				op = expr[i];
				i++;
				found = true;
			}
			else{
				i++;
				lenL++;
			}
		}
		char* LEFT = new char[lenL];
		for(int j=0;j<lenL;j++){
			LEFT[j]=expr[j+1];
		}
		lenR=len-3-lenL;
		char* RIGHT = new char[lenR];
		for(int j=0;j<lenR;j++){
			RIGHT[j]=expr[j+lenL+2];
		}
		node *opNODE = new node();
		string s(1, op);
		opNODE->data = s;
		BinTree* right = makeTree(RIGHT);
		BinTree* left = makeTree(LEFT);
		BinTree* newTree = makeBT(left, right, opNODE);
		return newTree;
	}
}

int main() {
	string s;
	getline(cin,s);
	char* expr = deleteSpace(s);
	BinTree* tree = makeTree(expr);
	cout << "tree in BFS traversal order: " << endl;
	BFSprint(tree->root);
	cout << "the result of arithmetic expression is: ";
	double result = interp(tree->root);
	cout << result << endl;
	//to make it doesn't close in exe file
	char a;
	cin >> a;
	return 0;
}