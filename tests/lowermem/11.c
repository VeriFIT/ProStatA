// almost the same structures, memcpy not eliminate
#include <string.h>
#include <stdio.h>

struct kel {
	int sk;
};

struct kapusta {
	int sk;
	int cz;
};

struct zeli {
	int cz;
};


int main() {
	struct kel a = {.sk = 3}; 

	struct kapusta b = {.sk = 4, .cz = 5};
	struct zeli c;

	printf("%d %d ", b.sk, b.cz);

	memcpy(&c, &(b.cz), sizeof(struct zeli));
	printf("%d ", c.cz);

	memcpy(&b, &a, sizeof(struct kel));
	printf("%d ", b.sk);

	memcpy(&a, &c, sizeof(struct kel));
	printf("%d ", a.sk);

	return 0;
}

