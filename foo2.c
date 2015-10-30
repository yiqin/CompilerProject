extern int foo2(int x);

extern int foo3(int x, int y);

int foo(int count)
{
    int i;
    int sum;
    int sum2;
    sum = 0;
    sum2 = 1;


    for    (i = 1; i <= count; i += 1)
        sum += foo2(i);


    return sum;
}
