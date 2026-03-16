int main() {
    int a;
    a = 10;
    {
        {
            a = a + 2;
            return a;
        }
        a = 100;
    }
    return 0;
}
