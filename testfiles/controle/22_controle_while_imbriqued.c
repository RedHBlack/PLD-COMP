int main()
{
    int x = 0;
    int y = 0;
    while (x < 3)
    {
        while (y < 2)
        {
            y = y + 1;
        }
        x = x + 1;
    }
    return x + y;
}