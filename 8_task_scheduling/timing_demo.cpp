#include <iostream>
#include <chrono>
int main() {
 auto start = std::chrono::steady_clock::now();
 volatile long long sum = 0;
 for (int i = 0; i < 10000000; ++i) {
 sum += i;
 }
 auto end = std::chrono::steady_clock::now();
 auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
 std::cout << "Elapsed time: " << elapsed_ms << " ms\n";
 return 0;
}