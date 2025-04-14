int add(int a, int b)
{
    return a + b;
}

int main()
{
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    return add(add(a * b, 2), add(add(c, 11), d));
}