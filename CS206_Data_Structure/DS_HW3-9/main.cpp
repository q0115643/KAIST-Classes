#include <iostream>

int cnt=0;
const char *a[3];

void move(int n, int l, int r){
	printf("\nMove %d from %s to %s", n, a[l], a[r]);
	cnt++;
}
void hanoi(int n, int l, int m, int r){  //the pieces in default are numbered 1, 2, 3, ... , n starting from the top
	if (n == 1){
		move(n, l, r);            //base-case : move one from left to right
	}
	else{
		hanoi(n - 1, l, r, m);    //move (n-1) pieces from left to middle
		move(n, l, r);            //move one piece from left to right
		hanoi(n - 1, m, l, r);    //move (n-1) pieces from middle to right
	}
}
int main() {
	int n;
	a[0] = "left";
	a[1] = "middle";
	a[2] = "right";
	printf("the pieces in default are numbered 1, 2, 3, ... , n starting from the top");
	printf("\ninsert the height N of your Hanoi tower : ");
	scanf("%d", &n);
	hanoi(n, 0,1,2);
	printf("\nthe height of tower was %d\nthe number of total moves is %d times", n, cnt);
	return 0;
}