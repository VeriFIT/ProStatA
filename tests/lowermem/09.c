// assign local variable
struct bus {
	int x;
	struct bus* y;
};

int main() {
	struct bus s = {.x = 1, .y = 0};
	struct bus item = {.x = 2, .y = 0};
	s.y = &item;

	return s.y->x;
}

