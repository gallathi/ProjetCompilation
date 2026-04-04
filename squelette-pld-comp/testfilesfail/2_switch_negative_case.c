int main() {
    int a = 0;
    int x = -1;
    switch (x) {
        case -2:
            a = 1;
            break;
        case -1:
            a = 2;
            break;
        case 0:
            a = 3;
            break;
        default:
            a = 4;
    }
    return a;
}