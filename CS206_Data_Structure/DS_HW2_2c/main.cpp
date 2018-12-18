#include <iostream>

#define max_queue_size 255

typedef struct element{
	int priority;
	char *name;
};

typedef struct Queue{
	element queue[max_queue_size];
	int front;
	int rear;
};

bool is_empty(Queue *q){
	return q->front == q->rear;
}

bool is_full(Queue *q){
	return (q->rear+1)%max_queue_size==q->front;
}

element Dequeue(Queue *q){
	if (is_empty(q)){
		printf("error: q is empty");
		exit(1);
	}
	q->front = (q->front+1) % max_queue_size;
	return q->queue[q->front];
}

void Enqueue(Queue *q, element item){
	if (is_full(q)){
		printf("error: q is full");
		exit(1);
	}
	if(q->rear==-1&&q->front==-1){
		q->rear++;
		q->front++;
		q->queue[0]=item;
	}
	else if(q->rear==0&&q->front==0){
		q->rear++;
		if(item.priority<q->queue[0].priority){
			q->queue[1] = q->queue[0];
			q->queue[0] = item;
		}
		else{
			q->queue[1] = item;
		}
	}
	else{
		int i = q->front;
		while(i!=q->rear&&q->queue[i].priority<=item.priority){ //finding place
			i++;
			if (i==max_queue_size){
				i=i%max_queue_size;
			}
		}
		if(i==q->rear) {
			if (q->queue[i].priority <= item.priority) {
				q->rear = (q->rear + 1) % max_queue_size;
				q->queue[q->rear] = item;
			}
			else{
				int before = q->rear;
				q->rear = (q->rear+1)%max_queue_size;
				q->queue[q->rear]=q->queue[before];
				q->queue[before]=item;
			}
		}
		else{
			int k = q->rear;
			q->rear = (q->rear+1) % max_queue_size;
			int here = i;
			while(k!=here){
				q->queue[(k+1)%max_queue_size]=q->queue[k];
				k--;
				if(k==-1){
					k = max_queue_size -1;
				}
			}
			q->queue[(k+1)%max_queue_size]=q->queue[k];
			q->queue[here]=item;
		}
	}
}

void getInputandEnqueue(Queue *q, char *input, int &nofPeople){
	fgets(input, 500, stdin);
	int i = 0;
	int length = 0;
	if(input[0] == '(') {
		while (input[i] != '\n') {
			i++;
			length++;
		}
		element item;
		item.name = input;
		printf("\n");
		item.priority = int(input[length - 2]) - 48;
		Enqueue(q, item);
		nofPeople++;
		char newIp[500];
		input = newIp;
		getInputandEnqueue(q, input, nofPeople);
	}
	else if(input[0]=='d'){
		if(input[1]=='o'&&input[2]=='n'&&input[3]=='e'&&input[4]=='\n'){
			return;
		}
		else{
			printf("error: wrong input\n");
			exit(1);
		}
	}
	else{
		printf("error: wrong input\n");
		exit(1);
	}
}
int main() {
	char input[500];
	Queue q;
	q.front = -1;
	q.rear = -1;
	int nofPeople=0;
	getInputandEnqueue(&q,input,nofPeople);
	printf("the output is : \n");
	for (int i=0;i<nofPeople;i++){
		printf("%s", q.queue[i].name);
	}
	return 0;
}