int f(int x) {
    return x + 1;
}

int g(int x) {
    return f(x) * 2;
}

int h(int x) {
	int a = 10, b;
	b = g(x);
    return a + b;
}

int main() {
    return h(5);
}
