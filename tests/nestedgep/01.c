struct inner {
    int a;
    int b;
};

struct outer {
    struct inner x;
    struct inner y;
};

#define offset_of(enclosing, field_name)\
    ((&( (enclosing*)0 )->field_name))

#define enclosing_ptr(enclosing, field_name, ptr)\
    ((enclosing*)((char*)(ptr) - (unsigned long)(offset_of(enclosing, field_name))))

void manipulate_xa(int *yb) {
    struct inner *inner_y = enclosing_ptr(struct inner, b, yb);
    struct outer *o = enclosing_ptr(struct outer, y, inner_y);
    o->x.a = 3; // exit value of process is set to 3
}

int main(int argc, char *argv[])
{
    struct outer value = {{20,21}, {30,31}};
    manipulate_xa(&value.y.b);
    return value.x.a;
}
