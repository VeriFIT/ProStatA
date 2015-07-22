extern void* krava;
void* bagr = &krava;
void* krava = &bagr;

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
