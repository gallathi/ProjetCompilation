int main() {
	int a = 1, b = 1, c = 0;
	if (a && !(++b || ++c)) {
		return b+2*c;
	} else {
		return b+2*c+10;
	}
}
