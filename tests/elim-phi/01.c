#include <stdbool.h>
bool func(bool r, bool y) {
	return y || r;
}

int main() {
	int b = 4;
	bool l = func(true,false);
	if (l)
		return b;
	else
		return 0;
}
