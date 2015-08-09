// delete copy source
#include <string.h>
#include <stdio.h>

struct bus {
	int x;
	struct bus* y;
};

int main() {
	struct bus src = {.x = 3, .y = 0}; 

	struct bus destiny;
	memcpy(&destiny, &src, sizeof(struct bus));
	printf("%d ", destiny.x);

	memset(&src, 0, sizeof(struct bus));
	printf("%d ", destiny.x);
	printf("%d ", src.x);

	return 0;
}

