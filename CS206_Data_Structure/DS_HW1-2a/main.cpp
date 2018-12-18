#include <iostream>

int main() {
	int arr[255];
	int idx = 0;
	bool putplus = false;
	printf("We'll get an input polynomial of the form\n");
	printf("p(x) = c1*x**e1 +  c2*x**e2 + . . . + cn*x**en\n");
	printf("enter your input : \n");

	while(arr[idx-1]!=-1 && idx < 20){
		int v = idx/2 + 1;
		if (idx%2==0) {
			printf("Enter e%d, Press -1 to finish:\n", v);
			scanf("%d", &arr[idx]);
			if(arr[idx]<-1){
				printf("error : e input should be non-negative integer!");
				return 0;
			}
			else if(idx>1){
				if(arr[idx]>arr[idx-2]){
					printf("error : e should be in decreasing order!!");
					return 0;
				}
			}
		}
		else {
			printf("Enter c%d\n", v);
			scanf("%d", &arr[idx]);
		}
		idx++;
	} //idx = number of input, idx/2 = number of pairs
	for(int i = 0; i < idx-1; i++) {
		if (i%2==0){
			printf("e%d: ", i/2+1);
		}
		else{
			printf("c%d: ", i/2+1);
		}
		printf("%d ", arr[i]);
	}
	idx=idx/2;
	//printing input
	printf("\n");

	printf("p1(x) = ");
	if (idx == 0){
		printf("0");
	}
	for(int i = 0; i < idx; i++){
		//c
		if(i!=0 && arr[2*i+1]>=0 && arr[2*i]>=0){
			if(arr[2*i-1]==0 && putplus){
				printf("+");
				putplus = false;
			}
			if(arr[2*i-1]!=0 && arr[2*i+1]!=0){
				printf("+");
			}
		}
		if (arr[2*i+1]!=0){
			printf("%d", arr[2 * i + 1]);
			putplus = true;
		}
		//e
		if (arr[2*i]!=0 && arr[2*i+1]!=0){
			printf("*x**%d", arr[2*i]);
		}
	}
	printf("\n");
		//c:arr[2*i+1]
		//e:arr[2*i]
		//at end, arr[2*i+2]==-1
		//print c
}