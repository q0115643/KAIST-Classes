#include <iostream>
using namespace std;
#define max_stack_size 500

class node{
public:
	int key;
	struct node *left;
	struct node *right;
	node(){
		key = 0;
		left = NULL;
		right = NULL;
	}
	node(int n){
		key = n;
		left = NULL;
		right = NULL;
	}
};

class Stack{
private:
	int top;
	node *sArray[max_stack_size];
public:
	Stack(){
		top=-1;
	}
	bool IsEmpty(){
		if(top==-1){
			return true;
		}else{
			return false;
		}
	}
	bool IsFull(){
		if(top==max_stack_size){
			return true;
		}
		return false;
	}
	void push(node *item){
		if(IsFull()){
			exit(1);
		}
		else{
			sArray[++top]=item;
		}
	}
	void pop(){
		if(IsEmpty()){
			printf("empty stack");
			exit(1);
		}
		top--;
	}
	node* TOP(){
		if(IsEmpty()){
			printf("empty stack");
			exit(1);
		}
		return sArray[top];
	}
};

class BinTree{
public:
	node *root;
	node *current;
	BinTree(){
		root=NULL;
		current=NULL;
	}
	bool IsEmpty(){
		if (root==NULL){
			return true;
		}else{
			return false;
		}
	}
	BinTree Lchild(){
		if (root==NULL){
			exit(1);
		}
		BinTree *Lsubtree = new BinTree();
		Lsubtree->root = root->left;
		return *Lsubtree;
	}
	BinTree Rchild(){
		if (root==NULL){
			exit(2);
		}
		BinTree *Rsubtree = new BinTree();
		Rsubtree->root = root->right;
		return *Rsubtree;
	}
	int Data(){
		if (root==NULL){
			exit(3);
		}
		int data = root->key;
		return data;
	}
	void InsertNode(node *item){						//insert in binary search tree's way
		if (root==NULL){								//if NULL, root.
			root=item;
		}else{
			node *prev = new node();					//make prev which saves the last current
			current = root;								//start current from root
			while(current){								//while current is not NULL
				prev = current;							//save current to prev
				if (current->key<item->key){			//if current's key is smaller than item's key item need to go right
					current=current->right;				//current go right
				}else{
					current=current->left;				//else current go left
				}
			}											//end when current is NULL <-place where item need to go in
			if (prev->key<item->key){					//recall that place with prev
				prev->right=item;
			}else{
				prev->left=item;
			}
		}
	}
	void preorder(){									//print in preorder
		if (root==NULL){
			return;										//if null, return
		}
		Stack *S = new Stack();							//make a stack
		S->push(root);									//push the root in it
		while(!S->IsEmpty()){								//loop while stack is not empty
			node *topNode = S->TOP();						//pop stack and save it as topnode
			S->pop();
			cout<<topNode->key<<"  ";						//print topnode
			if (topNode->right!=NULL){						//if right exist, save it in stack
				S->push(topNode->right);
			}
			if (topNode->left!=NULL){						//if left exist, save it in stack
				S->push(topNode->left);
			}											//saving in stack is right to left because it needs to be saved reversely
		}												//so the popped item will be left to right
	}
	void postorder(){									//print in postorder
		if (root==NULL){
			return;										//if null, return
		}
		Stack *S1 = new Stack();						//make stack 1
		Stack *S2 = new Stack();						//make stack 2
		current=root;									//save root to current
		S1->push(current);								//push root to stack 1
		while(!S1->IsEmpty()){								//loop while stack 1 is not empty
			node *tops1 = S1->TOP();						//pop stack 1 and save it in tops1
			S1->pop();
			S2->push(tops1);								//push tops1 in stack 2
			if (tops1->left!=NULL){							//push tops1's left to stack 1
				S1->push(tops1->left);
			}
			if (tops1->right!=NULL){						//push tops1's right to stack 1
				S1->push(tops1->right);
			}											//nodes will be saved in stack 2 reverse of post-order
		}												//parent->right->left (post-order is parent->right->parent)
		while(!S2->IsEmpty()){
			node *tops2 = S2->TOP();					//that's why stack 2 is used, make it reverse again and print
			S2->pop();
			cout<<tops2->key<<"  ";
		}
	}
};

int main() {
	int ITEM;
	BinTree *tree = new BinTree();
	int n;
	cout << "my tree will insert items like Binary Search Tree"<<endl;
	cout << "type in the number of numbers you want to input :  " <<endl;
	cin>> n;
	for (int i=0;i<n;i++) {
		cout << "type in " << i+1 << "-th your number :  " << endl;
		cin >> ITEM;
		node *NODE = new node(ITEM);
		tree->InsertNode(NODE);
	}
	cout << "----items in pre-order----" << endl;
	tree->preorder();
	cout << "\n---items in post-order---" << endl;
	tree->postorder();
	cout << "\n--------Thank You--------" << endl;
	char noend;
	cin >> noend;
	return 0;
}