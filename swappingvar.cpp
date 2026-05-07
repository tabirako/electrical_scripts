#include <iostream>
#include <chrono>
using namespace std;

int main() {
    volatile int a = 5, b = 10;

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; ++i) {
        int temp = a;
        a = b;
        b = temp;
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "temp swap: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " us\n";

    a = 5; b = 10;

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; ++i) {
        a ^= b;
        b ^= a;
        a ^= b;
    }
    end = chrono::high_resolution_clock::now();
    cout << "xor swap: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " us\n";

    return 0;
}