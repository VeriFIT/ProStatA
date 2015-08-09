// memset for array
#include <string.h>

int main() {
	int array[5];
	memset(array, 23, sizeof(array));
	return array[3];
}
