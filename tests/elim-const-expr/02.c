// constant expression as operand

#include <stdlib.h>

struct a {
	struct a* next;
} x = {.next = NULL};

int main() {

	if (x.next) {
		return 1;
	}

	return 0;
}

