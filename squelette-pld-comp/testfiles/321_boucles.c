int main() {
	int a, b, cpt;
	a = 10;
	b = 3;
	cpt = 0;
	
	if (a < b) {
		while(1) {
			cpt++;
			break;
		}
	} else {
		while(a > b) {
			while(1) {
				while(0) {
					cpt -= 50;
				}
				cpt += 1;
				if (cpt > b+3) {
					break;
				}
			}
			b += 1;
		}
	}
	return cpt;
}
/* cpt = 13 */
