int main()
{
    {
        int a;
        a = 2;
        {
            int b;
            b = 1;
            return b;
        }
        return a;
    }
    return 0;
}