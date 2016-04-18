// static local
int xfunc = 3;

int func() {
	static int xfunc = 5;
	return xfunc;
}

int main() {
	static int smain = 7;
	return smain + func();
}

