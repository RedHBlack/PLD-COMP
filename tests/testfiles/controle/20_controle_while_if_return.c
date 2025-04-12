int main()
{
	int x = 0;
	while (x < 10)
	{
		if (x == 5)
		{
			return 42;
		}
		x = x + 1;
	}
	return x;
}