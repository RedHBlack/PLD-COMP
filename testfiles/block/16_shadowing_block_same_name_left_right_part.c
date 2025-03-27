int main()
{
    int a = 10;
    {
        int a = 3;
        a = a;
    }
    return a;
}