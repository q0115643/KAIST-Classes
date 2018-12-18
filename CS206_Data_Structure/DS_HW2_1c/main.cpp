#include <iostream>

#define max_stack_size 255

typedef struct STACK {
	int stack[max_stack_size];
	int top = -1;
};

typedef int element;

bool IsEmpty(STACK *s){
	return (s->top)==-1;
}

bool IsFull(STACK *s){
	return (s->top)>=max_stack_size;
}

void push(STACK *s, element item){
	if (IsFull(s)){
		printf("stackoverflow\n");
		exit(1);
	}
	else{
		s->top++;
		s->stack[(s->top)]=item;
	}
}

element pop(STACK *s){
	if (IsEmpty(s)){
		printf("stack is empty\n");
		exit(1);
	}
	else {
		element returnelm = s->stack[s->top];
		s->top--;
		return returnelm;
	}
}

element topElm(STACK *s){
	if (IsEmpty(s)){
		printf("stack is empty\n");
		exit(1);
	}
	else {
		return s->stack[(s->top)];
	}
}


int getAE_length(char *question, char *input) {
	printf("%sMax %d characters for input\n", question, 255);
	fgets(input, 255, stdin);
	int i = 0;
	int length = 0;
	while (input[i] != '\n') {
		i++;
		length++;
		if(i>=255){
			printf("error: the input is oversize, give a new proper input\n");
			getAE_length(question, input);
		}
	}
	return length;
}

int prcd(char op){
	switch (op) {
		case '(':
		case ')':
			return 0;
		case '+':
		case '-':
			return 1;
		case '*':
		case '/':
			return 2;
		default:
			return -1;
	}
}

void postfixInput(char *postfixed, char *input, int length, int &pflength, int &nofdots){
	char last_op;
	char elm;
	STACK s;
	for (int i=0;i<length;i++){
		elm = input[i];
		switch (elm){
			case ' ' :
				break;
			case '(' :
				push(&s, elm);
				break;
			case ')' :
				last_op = pop(&s);
				while (last_op != '('){
					postfixed[pflength] = last_op;
					pflength++;
					last_op = pop(&s);
				}
				break;
			case '+' : case '-' : case '*' : case '/' :
				if ((!IsEmpty(&s))&&(prcd(topElm(&s))>=prcd(elm))) {
					last_op = pop(&s);
					postfixed[pflength] = last_op;
					pflength++;
				}
				push(&s, elm);
				break;
			default:
				if(elm=='0'|| elm=='1'|| elm=='2'|| elm=='3'|| elm=='4'|| elm=='5'|| elm=='6'|| elm=='7'|| elm=='8'|| elm=='9') {
					postfixed[pflength] = elm;
					pflength++;
					while (isdigit(input[i + 1])) {
						i++;
						postfixed[pflength] = input[i];
						pflength++;
					}
					postfixed[pflength] = '.';
					pflength++;
					nofdots++;
					break;
				}
				else{
					printf("error: wrong input here '%c'\n", elm);
					exit(1);
				}
		}
	}
	while (!IsEmpty(&s)){
		postfixed[pflength]=pop(&s);
		pflength++;
	}
}

int eval(char *postfixed, int pflength) {
	STACK s;
	int value;
	char elm;
	int num1;
	int num2;
	for (int i=0;i<pflength;i++){
		elm = postfixed[i];
		if (elm != '+' && elm != '-' &&  elm != '/' && elm != '*' ){
			int k = 1;
			while(isdigit(postfixed[i+k])){
				k++;
			}
			int numHere = 0;
			for (int j=0; j<k; j++){
				int tens=1;
				for (int m=0; m<k-1-j;m++){
					tens=tens*10;
				}
				numHere = numHere + (((int)postfixed[i+j])-48)*tens;
			}
			value = numHere;
			i=i+k;
			push(&s, value);
		}
		else{
			num2 = pop(&s);
			num1 = pop(&s);
			switch(elm) {
				case ('+'):
					value = num1 + num2;
					break;
				case ('-'):
					value = num1 - num2;
					break;
				case ('*'):
					value = num1 * num2;
					break;
				case ('/'):
					if (num2 == 0) {
						printf("error: you can't divide by 0");
						exit(1);
					}
					value = num1 / num2;
					break;
			}
			push(&s,value);
		}
	}
	return pop(&s);
}

int main() {

	int result;

	char input[255];
	char postfixed[255];
	int pflength=0;
	int nofdots=0;

	int length = getAE_length("\nplease type your infix Arithmetic Expression\n", input);
	postfixInput(postfixed, input, length, pflength, nofdots);
	printf("converted to postfix expression : ");
	for (int i=0;i<pflength;i++){
		if (postfixed[i]=='.'){
			printf(" ");
			continue;
		}
		printf("%c", postfixed[i]);
		switch(postfixed[i]){
			case('+'): case('-'): case('/'): case('*') : printf(" "); break;
		}
	}
	printf("\n");
	result = eval(postfixed, pflength);
	printf("the result is : %d\n", result);
	return 0;
}