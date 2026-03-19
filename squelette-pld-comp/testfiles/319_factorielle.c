int main() {
	int fact, n;
	fact = 1;
	n = 5;
	while (n > 0) {
		fact = fact * n;
		n--;
	}
	return fact;
}
