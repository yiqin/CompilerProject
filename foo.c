#include    "foo2.c"
#define min
#define max

extern int foo2(int x);

extern int foo3(int x, int y);

int foo(int count)
{
    int i, sum;
    int sum2;
    sum = 0;
    sum2 = 1;


    for    (i = 1; i <= count; i = i + 1)
        sum = sum + foo2(i);


    return sum;
}
