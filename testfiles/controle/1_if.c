int main() {
	int a = 1;
	int b = 2;
	if(a!=b) {
		b = 3;
		return b;
	} else if (a == b) {
		a = 4;
		return a;
	} else {
		return 1;
	}
	return 2;
}