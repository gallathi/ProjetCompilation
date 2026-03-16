int main() {
    int x;
    x = 1;
    {
        x = x + 1;
        return x;
    }
    x = 99;
    return 0;
}
