struct RT {
  char A;
  int B[10][20];
  char C;
};
struct ST {
  int X;
  double Y;
  struct RT Z;
};

int main(int argc, char** argv) {
	struct ST* s;
	int pejsek = 2 * 21;
  return &s[argc+pejsek].Z.B[5][13] == 0;
}
