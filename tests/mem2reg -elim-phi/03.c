// more complicate - 2 phi 
#include <stdio.h>

int main() {
	int p[10] = { 3, 4,56,12,34, 2, 9, 0,12,76};

	int i = 1;
	int max = 0;
	while (i < 10) {
		printf("%d-",p[i]);
		if (p[i] > max)
			max = p[i];
		i++;
	}
	printf("\nmax=%d\n",max);
	return 0;
}
