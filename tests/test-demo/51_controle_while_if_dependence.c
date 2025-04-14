int main()
{
	int x = 0;
	int y = 0;
	while (x < 10)
	{
		if (x == 5)
		{
			y = 42;
		}
		x = x + 1;
	}
	return y;
}