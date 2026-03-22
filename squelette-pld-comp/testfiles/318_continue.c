int main() {
	int somme, n;
	somme = 0;
	n = 0;
	while (n <= 10) {
		n += 1;
		if (n % 3 != 0) {
			continue;
		}
		somme += n;
	}
	return somme;
}
/* 18 */
