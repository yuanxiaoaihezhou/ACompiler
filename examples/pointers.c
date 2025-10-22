// Example: Pointer manipulation
// Demonstrates pointers and address operations

int main() {
    int x;
    int y;
    int *p;
    int *q;
    
    x = 10;
    y = 20;
    
    p = &x;
    q = &y;
    
    *p = *p + *q;
    
    return x;
}
