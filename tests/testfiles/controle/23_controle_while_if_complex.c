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