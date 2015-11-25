int printd( int i ) {
  return 1;
}

int foo(int a, int b) {
  return a;
}

int main() {
  int i,j;
  i = 450;
  j = -123;
  printd(i+j);
  /*
  printd(45000+j);
  printd(i+123);
  printd(45000+123);
  printd(i+(j+0));
  printd((i+0)+j);
  printd((i+0)+(j+0));
  printd((i+0)+123);
  printd(45000+(j+0));
  */
  return 0;
}
