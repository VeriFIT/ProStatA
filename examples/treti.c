int bagr = 5;

static int krava = 43;

struct mrkev
{
	int oranzova;
	int fialova;
};

struct zeli
{
	struct mrkev zelene;
	int cervene;
};

struct bus
{
	struct zeli kapusta;
	int x;
	struct bus* y;
};

int main()
{
	static int bum = 3;

	struct bus lala = {.x = 2, .y = 0};

	lala.kapusta.zelene.fialova = 1337;

	return 42;
}
