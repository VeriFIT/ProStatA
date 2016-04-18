// assign global variable
struct bus {
	int x;
	struct bus* y;
};

struct bus aglob;
struct bus bglob = {.x = 1, .y = 0};

int main() {
	aglob = bglob;
	bglob.x = 2;
	return aglob.x;
}

