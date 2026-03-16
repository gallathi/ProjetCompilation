int main()
{
    int a;
    a = 1;
    int counter;

    while (a <= 10)
    {
        if (a % 2 == 0)
        {
            counter = counter + 1;
        }

        a = a + 1;
    }

    return a;
}