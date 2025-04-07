int main()
{
	int x = 0;
	int y = 0;
	while (x < 10)
	{
		if (x % 2 == 0)
		{
			y = y + 2;
		}
		else
		{
			y = y + 1;
		}
		x = x + 1;
	}
	return y;
}

/*
La condition x < 10 est vérifiée dans BB_0, mais le programme retourne toujours à BB_0 sans jamais incrémenter correctement x ou sortir de la boucle.
Les blocs "vrai" et "faux" du if ne retournent pas correctement à la boucle externe.
L'incrémentation de x est soit absente, soit mal placée, ce qui empêche la condition x < 10 de devenir fausse.
*/