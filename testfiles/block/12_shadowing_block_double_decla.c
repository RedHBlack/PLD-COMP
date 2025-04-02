int main()
{
    int a;
    a = 9;
    {
        int a;
        a = 10;
        return a;
    }
    return a;
}