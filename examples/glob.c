struct bus
{
	int x;
	struct bus* y;
};


int iglob;
int eglob = 5; //extern?
const int cglob = 4; //extern?
int fglob = 2;
static int sglob = 0;
extern int exglob; //extern

struct bus ibus;
struct bus ebus = {.x = -1, .y = 0}; // extern?

char *nul_string = "aglob"; //extern?
int *ptrglob;

int func() {
	struct bus fbus = {.x = 3, .y = 0}; // privat const
	char *fstring = "afunc";
	return fglob + exglob;
}

int main() {
	static int sloc = 9;
	struct bus fibus;
	struct bus febus = {.x = eglob, .y = 0}; // non-const
	iglob = 1;
	return sglob + ebus.x;
}
