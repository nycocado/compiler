int main() {
    int a = 10;
    int b = 20;
    a += b;
    if (a > b) {
        calcula(a, b);
    } else {
        b = a << 2;
    }
    return 0;
}

int calcula(int x, int y) {
    int res = (x > y) ? x : y;
    while (x < 100) {
        x = x * 2;
    }
    return res;
}
