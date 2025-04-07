int main() {
	int x = 0;
	while (x < 10) {
		if (x == 9) {
			x = x + 10;
		}
		x = x + 1;
	}
	return x;
}