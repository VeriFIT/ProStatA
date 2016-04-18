// init big constant structure
#include <stdio.h>

struct bus {
	int x;
	struct bus* y;
};

struct big {
	int a;
	struct bus b;
	struct {
		char *c;
		double d;
		struct {
			struct bus *e;
			float f;
		};
	};
};

struct big x = {.a = 1, .b = {2, 0}, .c = "hrasko", .d = 3.14, .e = &x.b, .f = 6.1f};

int main() {
	printf("%d\n%d\n%s\n%g\n%s\n%d\n%g\n", 
	       x.a, x.b.x, x.c, x.d, (&x.b == x.e) ? "true" : "false", x.e->x, x.f);
	return 0;
}
