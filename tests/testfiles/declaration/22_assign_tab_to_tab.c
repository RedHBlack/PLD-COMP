int main()
{
    int a = 1;
    int b = 2;
    int tab[3];

    tab[1] = a;
    tab[2] = 2;

    tab[a] = b;

    int c = tab[a];

    int d = tab[1];

    d = tab[a];
    d = tab[1];

    tab[1] = tab[tab[a]];

    tab[tab[a]] = tab[1];

    return tab[a];
}