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
	//	printf("here1 with %c\n", item.name);
		q->rear++;
		q->front++;
		q->queue[0]=item;
	}
	else if(q->rear==0&&q->front==0){
	//	printf("here2 with %c\n", item.name);
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
	//	printf("here3 with %c\n", item.name);
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

int main() {
	element a;
	a.name = (char*)"john";
	a.priority = 5;
	element b;
	b.name = (char*)"holy";
	b.priority = 4;
	element c;
	c.name = (char*)"moly";
	c.priority = 1;
	element d;
	d.name = (char*)"yeah";
	d.priority = 2;
	Queue q;
	q.front = -1;
	q.rear = -1;
	Enqueue(&q, a);
	Enqueue(&q, b);
	Enqueue(&q, c);
	Enqueue(&q, d);
	Dequeue(&q);
	printf("%s ", q.queue[0].name);
	printf("%s ", q.queue[1].name);
	printf("%s ", q.queue[2].name);
	printf("%s\n", q.queue[3].name);
	printf("front : %d\n", q.front);
	printf("rear : %d\n", q.rear);


	return 0;
}