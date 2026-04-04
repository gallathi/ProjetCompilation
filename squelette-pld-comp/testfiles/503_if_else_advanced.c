int main()
{
    int a;
    a = 1;

    if (a == 1)
    {
        a = a + 1;
    }
    if (a == 2)
    {
        a = 3;
        if (a == 3)
        {
            {
                a = a - 2;
            }
            a = a + 3;
            if (a == 8)
            {
                a = a + 1;
            }
            else if (a == 4)
            {
                a = a + 2;
            }
        }
        if (a == 1000)
        {
            a = a + 1;
        }
        else if (a = 6)
        {
            a = a + 8;
        }
    }

    return a;
}