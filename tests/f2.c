
float getValue(int n)
{
    float x;
    switch (n)
    {
        case 1:
            x = 3.14;
        case 2:
            x = 2.78;
        default:
            x = 6.022E23;
    }
    return x;
}

int hint()
{
    int a = 3;
    return a + 7;
}

void main()
{
    float a, b, c;

    a = b = 5.2;
    b += getValue(1) + hint();
}
