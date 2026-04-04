int fact(int n) {
    if (n <= 1) { return 1; }
    return n * fact(n - 1);
}

int square(int n) {
    return n * n;
}

void print_int(int n) {
    if (n >= 10) {
        print_int(n / 10);
    }
    putchar(n % 10 + '0');
}

int main() {
    int choice, charValue, intValue, result;

    choice = getchar();
    charValue = getchar();

    intValue = charValue - '0';

    switch (choice) {
        case 102:
            result = fact(intValue);
            break;
        case 115:
            result = square(intValue);
            break;
        default:
            result = 0;
    }

    print_int(result);
    putchar(10);

    return 0;
}