int main()
{
    int a = 1;
    int b = 3;
    int tab[3];

    tab[1] = a;

    tab[a] = b;

    int c = tab[a];

    int d = tab[1];

    d = tab[a];
    d = tab[1];

    tab[1] = tab[tab[a]];

    tab[tab[a]] = tab[1];

    return tab[a];
}