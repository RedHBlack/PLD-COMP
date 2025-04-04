int main() {
    int a = 2;
    int tab[3];
    tab[2] = a;
    tab[tab[tab[a]]] = 5;

    return 0;
}