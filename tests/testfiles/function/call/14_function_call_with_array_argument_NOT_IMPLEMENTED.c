int sumArray(int arr[], int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
    }
    return sum;
}

int main()
{
    int numbers[] = {1, 2, 3, 4};
    int result = sumArray(numbers, 4);
    return 0;
}