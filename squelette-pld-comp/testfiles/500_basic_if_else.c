int main()
{
    int a;
    a = 1;

    int v;
    v = 1;
    {
        int p;
        p = 2;
    }
    if (a == a)
    {
        int b;
        b = 1;
    }
    else if (a == a)
    {
        int c;
        c = 2;
    }

    else
    {
        int i;
        i = 1;
    }

    a = 2;
    return a;
}