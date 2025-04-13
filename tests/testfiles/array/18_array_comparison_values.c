int main()
{
    int tab1[3] = {1, 2, 3};
    int tab2[3] = {1, 2, 3};
    int areEqual = 1;

    if (tab1[0] != tab2[0])
    {
        areEqual = 0;
    }
    if (tab1[1] != tab2[1])
    {
        areEqual = 0;
    }
    if (tab1[2] != tab2[2])
    {
        areEqual = 0;
    }

    return areEqual;
}