// insert into array
#include <string.h>
#include <stdio.h>

int main() {
	char *from = "ial";
	char to[9] = "kolonada";
	memcpy(&to[5],from,4);
	printf("%s+%s\n",from,to);

	return 0;
}

