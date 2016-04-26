// global vars as constant expression lvl 3
struct bus {
	int x;
	struct bus* y;
};

struct big {
	int a;
	struct bus b;
	int *c;
};

struct big x = {.a = 1, .b = {2, 0}, .c = &x.b.x};

int main() {
	return *x.c;
}

