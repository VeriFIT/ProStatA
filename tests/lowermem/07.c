// different types
#include <string.h>
#include <stdio.h>

struct bus {
	int x;
	struct bus* y;
};

int main() {
	char *from = "abc";
	char to[10];
	memcpy(&to,from,4);
	printf("%s+%s\n",from,to);

	return 0;
}


