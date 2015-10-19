extern int foo2(int x);

int foo(int count) 
{
    int sum = 0.0;

    for(int i=1; i <= count; i++) 
        sum += foo2(i);
    return sum; 
}