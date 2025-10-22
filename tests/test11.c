// Test comparison operators
int main() {
    int a;
    int result;
    a = 10;
    result = 0;
    
    if (a > 5)
        result = result + 1;
    if (a >= 10)
        result = result + 1;
    if (a < 20)
        result = result + 1;
    if (a <= 10)
        result = result + 1;
    if (a != 5)
        result = result + 1;
    
    return result;
}
