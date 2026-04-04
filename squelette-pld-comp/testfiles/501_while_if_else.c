int main()
{
    int a;
    a = 1;
    int counter;
    counter = 1;

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
