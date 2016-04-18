// init constant structure
struct choice {
	_Bool isArr;
	union {
		int num;
		int arr[5];
	};
};

struct choice x = {.isArr = 1, .arr = {1,1,2,3,5} };

int main() {
	if (x.isArr)
		return x.arr[2];
	else
		return x.num;
}
