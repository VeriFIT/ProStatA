int bagr = 5;

static int krava = 43;

struct bus
{
	int x;
	int y;
};

int lala_je_h__zl(int x)
{
	return x * 2;
}

int main()
{
	static int bum = 3;

	struct bus lala = {.x = 2, .y = 6};

	int y = lala_je_h__zl(42);

	return y;
}
