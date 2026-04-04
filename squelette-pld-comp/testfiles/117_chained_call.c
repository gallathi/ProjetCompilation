int f(int x) {
    return x + 1;
}

int g(int x) {
    return f(x) * 2;
}

int h(int x) {
    return g(x) + 10;
}

int main() {
    return h(5);
}
