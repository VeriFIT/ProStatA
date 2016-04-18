// global init
struct bus {
	int x;
	struct bus* y;
};

int eglob = 1; 
static int sglob = 2;
struct bus ebus = {.x = 3, .y = 0}; 

int main() {
	struct bus febus = {.x = eglob, .y = 0}; // non-const
	return sglob + febus.x + ebus.x;
}

