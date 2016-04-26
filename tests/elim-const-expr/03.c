// constant expression in phi
#include <stdio.h>

int main() {
	int i = 5;
	char *str = i % 2 == 0 ? "a" : i % 3 == 0 ? "b" : i % 5 == 0 ? "c" : i % 7 == 0 ? "d" : "e";
	printf("%s\n",str);
	return 0;
}

