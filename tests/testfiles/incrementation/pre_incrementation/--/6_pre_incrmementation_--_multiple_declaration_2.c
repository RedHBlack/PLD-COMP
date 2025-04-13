int main()
{
    int a = 5;
    int b = --a;
    int c = --a;
    int d = --b - --c;
    return d;
}