
int soma = 0;

int valor(int p, int q, int r) { return p > q ? r / 3 : p + r * 5; }

int rec(int n)
{
    soma += rec(n - 1);
    return soma;
}

int main()
{
    int a, b, c, d, e = 0, f = 1;
    a = 2;
    b = -1;
    d = 12 & 24 + 1;
    c = b + rec(a) * rec(d);
    if (a < d)
        e = a >= b || b < 3 ? c : f / d - 3;
    else
        f -= a == d ? ~c & b : 6;
    return e - a % c * f;
}
