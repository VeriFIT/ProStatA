const int bagr = 5;

static int krava = 43;
int lama2 = bagr;

struct bus
{
	int x;
	int y;
};

int main()
{
	static int bum = 3;

	struct bus lala = {.x = bum};
}
