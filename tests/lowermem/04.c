// zeroinitializer structure
#include <string.h>

struct bus {
	int x;
	struct bus* y;
};

int main() {
	struct bus trup = {.x = 0, .y = 0};
	return trup.x;
}


