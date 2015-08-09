// init structure
#include <string.h>

struct bus {
	int x;
	struct bus* y;
};

int main() {
	struct bus uroboros = {.x = 4, .y = &uroboros};
	return uroboros.y->x;
}


