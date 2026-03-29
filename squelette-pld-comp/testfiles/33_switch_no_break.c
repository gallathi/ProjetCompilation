int main()
{
    int a=0;
    switch (a)
    {
        case 0:
            a+=1;
        case 1:
            a+=1;
        default:
            a+=4;
    }
    return a;
}