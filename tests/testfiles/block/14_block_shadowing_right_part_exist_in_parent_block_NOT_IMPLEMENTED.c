int main()
{
    int b;
    b = 1;
    {
        int a;
        a = b;
        return a;
    }
    return 0;
}