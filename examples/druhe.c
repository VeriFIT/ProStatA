int bagr = 5;

static int krava = 43;

struct bus
{
	int x;
	struct bus* y;
};

int main()
{
	static int bum = 3;

	struct bus lala = {.x = 2, .y = 0};

	int krtek = lala.y->y->y->y->x = 1337;

	return 42;
}
