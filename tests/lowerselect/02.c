// multi ternary expression

int main() {
	int a = 5;
	int b = ( (a > 5)? ( (a > 6)? 100 : 10 ) : 
	                   ( (a == 5)? 5 : -100 ) );
/*	int b;
	if (a > 5) {
		if (a > 6) {
			b = 100;
		} else {
			b = 10;
		}
	} else {
		if(a == 5) {
			b = 5;
		}
		else {
			b = -100;
		}
	}
*/
	return b;
}

