// init constant structure
struct point {
	union {
		struct {
			double x;
			double y;
			double z;
		};
		double raw[3];
	};
};

struct point A = {.x=3,.y=2,.z=8};
struct point B = {.raw={7, 4, 1} };
struct point C = {.raw={0, 4, 1}, .x=1 };

int main() {
	return A.raw[0] + A.raw[1] + A.raw[2] +
	       B.x + B.y + B.z + C.x;
}
