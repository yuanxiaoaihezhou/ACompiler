// Example: Greatest Common Divisor (Euclidean algorithm)
// Demonstrates while loops and modulo operation

int gcd(int a, int b) {
    int temp;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int main() {
    return gcd(48, 18);
}
