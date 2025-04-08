int main()
{
    int b;
    b = 1;
    {
        int a;
        int b;
        b = 2;
        a = b;
        return a;
    }
    return 0;
}