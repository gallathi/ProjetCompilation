int addone(int x)
{
    return x + 1;
}

int addtwo(int x)
{
    return addone(addone(x));
}

int main()
{
    return addtwo(40);
}
