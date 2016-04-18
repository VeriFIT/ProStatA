// init constant array
#include <stdio.h>

int m[][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };

int main() {
	printf("%d %d %d\n%d %d %d\n%d %d %d\n",
	       m[0][0], m[0][1], m[0][2], 
	       m[1][0], m[1][1], m[1][2], 
	       m[2][0], m[2][1], m[2][2]);
	return 0;
}
