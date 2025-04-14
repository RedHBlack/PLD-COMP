int main()
{
    int a = 1;
    int b = (a && (a = 10));
    return a;
}