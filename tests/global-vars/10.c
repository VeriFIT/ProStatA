// init union with padding
#include <stdio.h>

union data {
	int i;

	struct {
		char c1;
		char c2;
		char c3;
	};
};
union data a = { .c1 = 1 };

int main() {
	union data b = { .c2 = 1 };
	printf("%d %d\n", a.i, b.i);
	return 0;
}
