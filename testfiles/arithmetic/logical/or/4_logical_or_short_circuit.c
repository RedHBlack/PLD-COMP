int main() {
    int a = 1;
    int b = (a || (a = 10));  // `a = 10` should NOT execute
    return a;  // Expected return value: 1 (a should remain 1)
}