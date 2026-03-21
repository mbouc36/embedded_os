#include <iostream>
#include <chrono>
#include <thread>
using namespace std::chrono_literals;
int main() {
 auto base = std::chrono::steady_clock::now();
 auto major_cycle = 1000ms;
 for (int cycle = 0; cycle < 5; ++cycle) {
 auto cycle_start = base + cycle * major_cycle;
 auto t1 = cycle_start + 0ms;
 auto t2 = cycle_start + 200ms;
 auto t3 = cycle_start + 500ms;
 std::this_thread::sleep_until(t1);
 std::cout << "Cycle " << cycle << ": Task 1 at 0 ms\n";
 std::this_thread::sleep_for(50ms);
 std::this_thread::sleep_until(t2);
 std::cout << "Cycle " << cycle << ": Task 2 at 200 ms\n";
 std::this_thread::sleep_for(70ms);
 std::this_thread::sleep_until(t3);
 std::cout << "Cycle " << cycle << ": Task 3 at 500 ms\n";
 std::this_thread::sleep_for(100ms);
 }
 return 0;
}