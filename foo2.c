extern int foo2(int x);

extern int foo3(int x, int y);

int foo(int count) 
{
    int sum = 0.0;
    int sum2 = 1.0;

    for(int i=1; i <= count; i++) 
        sum += foo2(i);


    return sum; 
}