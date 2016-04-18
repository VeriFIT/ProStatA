// address
extern void* krava;
void* bagr = &krava;
void* krava = &bagr;

int main() {
	if (bagr == krava)
		return 0;
	else
		return 1;
}
