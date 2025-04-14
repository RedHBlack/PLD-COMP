int main() {
    int x = 0;
    if (2 == 2) {
        x = 10;
        if (1 != 2) {
            x = 20;
            return x;
        }
    } else {
        x = 14;
        return x;
    }
    x = 20 + 10;
    x = x / 2;
    return x;
}