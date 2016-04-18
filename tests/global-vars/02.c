// global non-init
struct bus {
	int x;
	struct bus* y;
};


int iglob;
struct bus ibus;
int *ptrglob;

int main() {
	struct bus fibus;
	iglob = 1;
	ibus.x = 2;
	ibus.y = &fibus;
	ptrglob = &iglob;
	fibus.x = *ptrglob;
	return iglob + ibus.y->x;
}

