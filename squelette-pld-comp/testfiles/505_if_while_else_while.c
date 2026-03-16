int main()
{
    int a;
    a = 2;
    int b;
    b = 0;

    if (a == 2)
    {
        while (b < 10)
        {
            b = b + 1;
        }
    }
    else
    {
        while (a < 10)
        {
            a = a + 1;
        }
    }

    return a + b;
}