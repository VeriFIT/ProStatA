// constant expression as argument call instruction

int j(int *x, int y) {
	return *x + y;
}

int main() {
	int n[] = {1,2,3};
	int a = j(&n[2], n[1]);
	return a;
}
