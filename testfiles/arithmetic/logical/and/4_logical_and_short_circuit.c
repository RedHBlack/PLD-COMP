int main() {
    int a = 0;
    int b = (a && (a = 10));  // `a = 10` should NOT execute
    return a;  // Expected return value: 0 (a should remain 0)
}