#include <stdio.h>

int main() {
	int choix, reponse;
	reponse = 564;
	choix = 312;
	if (choix == reponse) {
		putchar('=');
		putchar('\n');
	} else if (choix < reponse) {
		putchar('+');
		putchar('\n');
	} else {
		putchar('-');
		putchar('\n');
	}
	return 0;
}
