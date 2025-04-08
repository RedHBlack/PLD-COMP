int mult(int a, int b)
{
    return a * b;
}

int squared(int n)
{
    mult(n, n);

    return n;
}

int main()
{
    return squared(5);
}