int main() {
    int x;
    x = 5;
    {
        int y;
        y = x + 1;
        x = y;
    }
    return x;
}
