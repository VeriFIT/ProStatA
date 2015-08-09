// copy into structure
#include <string.h>
#include <stdio.h>

struct bus {
	int x;
	struct bus* y;
};

struct krab {
	char x;
	struct bus y;
	double z;
};


int main() {
	struct bus structure = {.x = 3, .y = 0}; 
	struct krab socha = {.x = 'a', .y = {.x = 5, .y = 0}, .z = 3.14};
	struct krab david;

	struct bus trup;
	memcpy(&trup, &structure, sizeof(struct bus));
	printf("%d ", trup.x);

	memcpy(&trup, &socha.y, sizeof(trup));
	printf("%d ", trup.x);

	memset(&trup, 0, sizeof(struct bus));
	printf("%d ", trup.x);

	memcpy(&david.y, &socha.y, sizeof(david.y));
	printf("%d ", david.y.x);

	return 0;
}

