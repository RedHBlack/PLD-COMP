int main()
{
    int a = 10;
    {
        int a;
        a = a;
    }
    return a;
}