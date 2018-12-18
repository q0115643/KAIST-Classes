#include <iostream>
#define max_stack_size 255
typedef int element;
typedef struct Stack{
	int top;
	element stack[max_stack_size];
};
void CreateS(Stack *s) {
	s->top = -1;
}
bool IsFull(Stack *s){
	return (s->top)>=max_stack_size-1;
}
bool IsEmpty(Stack *s){
	return (s->top)==-1;
}
void push(Stack *s, element item){
	if (IsFull(s)){
		printf("error: stackoverflow\n");
		exit(1);
	}
	else{
		s->top++;
		s->stack[(s->top)]=item;
	}
}
element pop(Stack *s){
	if (IsEmpty(s)){
		printf("error: stack is empty, can't pop\n");
		exit(1);
	}
	else {
		element returnVal = s->stack[s->top];
		s->top--;
		return returnVal;
	}
}
int main() {
	Stack s;
	CreateS(&s);
	push(&s, (element)(int)1);
	printf("first push element: %d\n", s.stack[s.top]);
	push(&s, (element)(int)2);
	printf("second push element: %d\n", s.stack[s.top]);
	push(&s, (element)(int)3);
	printf("third push element: %d\n", s.stack[s.top]);

	printf("first popped element: %d\n", pop(&s));
	printf("second popped element: %d\n", pop(&s));
	printf("third popped element: %d\n", pop(&s));
	return 0;
}