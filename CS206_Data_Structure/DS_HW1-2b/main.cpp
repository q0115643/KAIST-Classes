#include <iostream>

int main() {
	int arr1[255];
	int arr2[255];
	int addarr[255];
	int dfrarr1[255];
	int dfrarr2[255];
	int idx1 = 0;
	int idx2 = 0;
	bool putplus = false;
	printf("We'll get two input polynomials of the form\n");
	printf("p(x) = c1*x**e1 +  c2*x**e2 + . . . + cn*x**en\n");
	printf("enter your input \n");

	while(arr1[idx1-1]!=-1 && idx1 < 20){
		int v = idx1/2 + 1;
		if (idx1%2==0) {
			printf("Enter e%d, Press -1 to input p2(x):\n", v);
			scanf("%d", &arr1[idx1]);
			if(arr1[idx1]<-1){
				printf("error : e input should be non-negative integer!");
				return 0;
			}
			else if(idx1>1){
				if (arr1[idx1] >= arr1[idx1 - 2]){
					printf("error : e should be in decreasing order!!");
					return 0;
				}
			}
		}
		else {
			printf("Enter c%d\n", v);
			scanf("%d", &arr1[idx1]);
		}
		idx1++;
	} //idx = number of input, idx/2 = number of pairs
	for(int i = 0; i < idx1-1; i++) {
		if (i%2==0){
			printf("e%d: ", i/2+1);
		}
		else{
			printf("c%d: ", i/2+1);
		}
		printf("%d ", arr1[i]);
	}//printing input
	printf("\n");

	while(arr2[idx2-1]!=-1 && idx2 < 20){
		int v = idx2/2 + 1;
		if (idx2%2==0) {
			printf("Enter e%d, Press -1 to finish:\n", v);
			scanf("%d", &arr2[idx2]);
			if(arr2[idx2]<-1){
				printf("error : e input should be non-negative integer!");
				return 0;
			}
			else if(idx2>1){
				if(arr2[idx2]>=arr2[idx2-2]){
					printf("error : e should be in decreasing order!!");
					return 0;
				}
			}
		}
		else {
			printf("Enter c%d\n", v);
			scanf("%d", &arr2[idx2]);
		}
		idx2++;
	} //idx = number of input, idx/2 = number of pairs
	for(int i = 0; i < idx2-1; i++) {
		if (i%2==0){
			printf("e%d: ", i/2+1);
		}
		else{
			printf("c%d: ", i/2+1);
		}
		printf("%d ", arr2[i]);
	}//printing input
	printf("\n");
	idx1=idx1/2;
	idx2=idx2/2;

	//add arrays + selection sort + combine duplicates
	int idx=idx1+idx2;
	for(int i=0; i<idx1+idx2; i++){
		if(i<idx1){
			addarr[2*i]=arr1[2*i];
			addarr[2*i+1]=arr1[2*i+1];
		}
		else{
			int j=i-idx1;
			addarr[2*i]=arr2[2*j];
			addarr[2*i+1]=arr2[2*j+1];
		}
	}
	//selection sort
	int indexMax = 0;
	for (int i=0;i<idx;i++){
		indexMax = i;
		for(int j = i+1;j<idx;j++) {
			if(addarr[2*j] > addarr[2*i]) {
				indexMax = j;
			}
		}
		if(indexMax != i) {
			// swap the numbers
			int temp = addarr[2*indexMax];
			addarr[2*indexMax] = addarr[2*i];
			addarr[2*i] = temp;
			int temp2 = addarr[2*indexMax+1];
			addarr[2*indexMax+1] = addarr[2*i+1];
			addarr[2*i+1] = temp2;
		}
	}
	//combine numbers
	for(int j=0;j<10;j++){
		for (int i=0;i<idx;i++) {
			if (addarr[2 * i] == addarr[2 * i + 2]) {
				addarr[2 * i + 1] = addarr[2 * i + 1] + addarr[2 * i + 3];
				addarr[2 * i + 3] = 0;
			}
		}
	}


	int idx3=0;
	for(int i = 0; i < idx1; i++){
		dfrarr1[2*i] = arr1[2*i]-1;
		dfrarr1[2*i+1] = arr1[2*i+1]*arr1[2*i];
		idx3++;
	}
	int idx4=0;
	for(int i = 0; i < idx2; i++){
		dfrarr2[2*i] = arr2[2*i]-1;
		dfrarr2[2*i+1] = arr2[2*i+1]*arr2[2*i];
		idx4++;
	}

	//mutiply array + sort elements + combine numbers with same e
	int mltarr[255];
	int idx5=0;
	for(int i=0; i<idx1; i++){
		for(int j=0; j<idx2; j++){
			int k=i*idx2+j;
			mltarr[2*k]=(arr1[2*i])+(arr2[2*j]);
			mltarr[2*k+1]=(arr1[2*i+1])*(arr2[2*j+1]);
			idx5++;
		}
	}
	//selection sort
	int indexMax2 = 0;
	for (int i=0;i<idx5;i++){
		indexMax2 = i;
		for(int j = i+1;j<idx5;j++) {
			if(mltarr[2*j] > mltarr[2*i]) {
				indexMax2 = j;
			}
		}
		if(indexMax2 != i) {
			// swap the numbers
			int temp = mltarr[2*indexMax2];
			mltarr[2*indexMax2] = mltarr[2*i];
			mltarr[2*i] = temp;
			int temp2 = mltarr[2*indexMax2+1];
			mltarr[2*indexMax2+1] = mltarr[2*i+1];
			mltarr[2*i+1] = temp2;
		}
	}
	//combine numbers
	for(int j=0;j<10;j++){
		for (int i=0;i<idx5;i++) {
			if (mltarr[2 * i] == mltarr[2 * i + 2]) {
				mltarr[2 * i + 1] = mltarr[2 * i + 1] + mltarr[2 * i + 3];
				mltarr[2 * i + 3] = 0;
			}
		}
	}

	// start printing results

	printf("p1(x) = ");
	if (idx1 == 0){
		printf("0");
	}
	for(int i = 0; i < idx1; i++){
		//c
		if(i!=0 && arr1[2*i+1]>=0){
			if(arr1[2*i-1]==0 && putplus){
				printf("+");
				putplus = false;
			}
			if(arr1[2*i-1]!=0 && arr1[2*i+1]!=0){
				printf("+");
			}
		}
		if (arr1[2*i+1]!=0){
			printf("%d", arr1[2 * i + 1]);
			putplus = true;
		}
		//e
		if (arr1[2*i]!=0 && arr1[2*i+1]!=0){
			printf("*x**%d", arr1[2*i]);
		}
	}
	putplus = false;
	printf("\n");
	printf("p2(x) = ");
	if (idx2 == 0){
		printf("0");
	}
	for(int i = 0; i < idx2; i++){
		//c
		if(i!=0 && arr2[2*i+1]>=0){
			if(arr2[2*i-1]==0 && putplus){
				printf("+");
				putplus = false;
			}
			if(arr2[2*i-1]!=0 && arr2[2*i+1]!=0){
				printf("+");
			}
		}
		if (arr2[2*i+1]!=0){
			printf("%d", arr2[2 * i + 1]);
			putplus = true;
		}
		//e
		if (arr2[2*i]!=0 && arr2[2*i+1]!=0){
			printf("*x**%d", arr2[2*i]);
		}
	}
	putplus = false;
	printf("\n");
	printf("p1(x)+p2(x) = ");
	if (idx == 0){
		printf("0");
	}
	for(int i=0; i<idx; i++) {
		//c
		if (i != 0 && addarr[2 * i + 1] >= 0) {
			if (addarr[2 * i - 1] == 0 && putplus) {
				printf("+");
				putplus = false;
			}
			if (addarr[2 * i - 1] != 0 && addarr[2 * i + 1] != 0) {
				printf("+");
			}
		}
		if (addarr[2 * i + 1] != 0) {
			printf("%d", addarr[2 * i + 1]);
			putplus = true;
		}
		//e
		if (addarr[2 * i] != 0 && addarr[2 * i + 1] != 0) {
			printf("*x**%d", addarr[2 * i]);
		}
	}
	putplus = false;
	printf("\n");
	printf("p1(x)*p2(x) = ");
	if (idx5 == 0){
		printf("0");
	}
	for(int i = 0; i < idx5; i++){
		//c
		if(i!=0 && mltarr[2*i+1]>=0){
			if(mltarr[2*i-1]==0 && putplus){
				printf("+");
				putplus = false;
			}
			if(mltarr[2*i-1]!=0 && mltarr[2*i+1]!=0){
				printf("+");
			}
		}
		if (mltarr[2*i+1]!=0){
			printf("%d", mltarr[2 * i + 1]);
			putplus = true;
		}
		//e
		if (mltarr[2*i]!=0 && mltarr[2*i+1]!=0){
			printf("*x**%d", mltarr[2*i]);
		}
	}
	putplus = false;
	printf("\n");
	printf("p1'(x) = ");
	if (idx3 == 0){
		printf("0");
	}
	for(int i = 0; i < idx3; i++){
		//c
		if(i!=0 && dfrarr1[2*i+1]>=0 && dfrarr1[2*i]>=0){
			if(dfrarr1[2*i-1]==0 && putplus){
				printf("+");
				putplus = false;
			}
			if(dfrarr1[2*i-1]!=0 && dfrarr1[2*i+1]!=0){
				printf("+");
			}
		}
		if (dfrarr1[2*i+1]!=0){
			printf("%d", dfrarr1[2 * i + 1]);
			putplus = true;
		}
		//e
		if (dfrarr1[2*i]!=0 && dfrarr1[2*i+1]!=0){
			printf("*x**%d", dfrarr1[2*i]);
		}
	}
	putplus = false;
	printf("\n");
	printf("p2'(x) = ");
	if (idx4 == 0){
		printf("0");
	}
	for(int i = 0; i < idx4; i++){
		//c
		if(i!=0 && dfrarr2[2*i+1]>=0 && dfrarr2[2*i]>=0){
			if(dfrarr2[2*i-1]==0 && putplus){
				printf("+");
				putplus = false;
			}
			if(dfrarr2[2*i-1]!=0 && dfrarr2[2*i+1]!=0){
				printf("+");
			}
		}
		if (dfrarr2[2*i+1]!=0){
			printf("%d", dfrarr2[2 * i + 1]);
			putplus = true;
		}
		//e
		if (dfrarr2[2*i]!=0 && dfrarr2[2*i+1]!=0){
			printf("*x**%d", dfrarr2[2*i]);
		}
	}
	printf("\n");
}