int putchar(int c);

void greaterOrLower(int a, int b)
{
    if (a > b)
    {
        putchar('G');
        return;
    }
    else
    {
        putchar('L');
        return;
    }

    putchar('B');
}

int main()
{
    greaterOrLower(5, 7);
    greaterOrLower(8, 7);
    return 0;
}