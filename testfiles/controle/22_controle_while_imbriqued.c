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

// RUNS INFINITELY
/*
La boucle interne (while (y < 2)) ne termine jamais correctement ou ne retourne pas à la boucle externe.
Une fois que y atteint 2, le programme saute à BB_5, mais y n'est jamais réinitialisé à 0. Cela empêche la boucle interne de fonctionner correctement lors des itérations suivantes.
Le programme reste bloqué dans une boucle infinie entre BB_0 et BB_5.
 */