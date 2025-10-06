int s = 5;

int fib_recursion(int n) {
  if (n <= 1) return 1;
  return fib_recursion(n - 2) + fib_recursion(n - 1);
}

int main() {
  return fib_recursion(s);
}
