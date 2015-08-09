// set zero global variable
#include <string.h>

struct bus {
	int x;
	struct bus* y;
};

struct bus glob = {.x = 1, .y = &glob};

int main() {
	int ret = glob.y->x;
	memset(&glob, 0, sizeof(struct bus));
	if (ret != 0)
		ret = glob.x;
	return ret;
}

