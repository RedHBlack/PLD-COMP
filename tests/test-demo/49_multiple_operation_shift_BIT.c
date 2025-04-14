int main()
{
    int a = 10;
    int b = 20;
    int c = 2;

    int result = ((a << c) & b) | ((b >> c) & a);

    return result;
}