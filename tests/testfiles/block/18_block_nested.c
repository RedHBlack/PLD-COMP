int main()
{
    {
        int a = 1;
        {
            int b = 2;
            {
                int c = 3;
                return c;
            }
        }
    }
    return 0;
}