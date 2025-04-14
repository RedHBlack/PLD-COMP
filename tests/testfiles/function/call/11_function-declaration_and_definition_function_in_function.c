int mult(int a, int b)
{
    return a * b;
}

int squared(int n)
{

    return mult(n, n);
}

int main()
{
    int a = 3 * 2;
    int b = 5 / 3;

    return squared(a + b) - squared(3);
}