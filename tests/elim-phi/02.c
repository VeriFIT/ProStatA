int main() {
	int i = 5;
	int *b = &i;
	int j = (i > 5)? 1 : ( (i == 5)? *b : 3 );
	return j;
}
