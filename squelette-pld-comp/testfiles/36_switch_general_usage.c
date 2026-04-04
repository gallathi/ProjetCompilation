int factorial(int n)
{
    if (n > 1)
    {
        return n * factorial(n-1);
    }
    return 1;
}

int main()
{
    int param = 5, choice = 2, result = 0;
    switch (choice)
    {
        case 0:
            result = param * param;
            break;
        case 1:
            result = param * 2;
            break;
        case 2:
            result = factorial(param);
            break;
        default:
            result = param;
            break;
    }
    return result;
}