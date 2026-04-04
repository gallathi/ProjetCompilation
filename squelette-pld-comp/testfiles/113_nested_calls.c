int add(int a, int b) {
    return a + b;
}

int multiply_sum(int x, int y, int z) {
    int sum;
    sum = add(x, y);
    return sum * z;
}

int main() {
    return add(2, multiply_sum(3, 4, 2));
}
