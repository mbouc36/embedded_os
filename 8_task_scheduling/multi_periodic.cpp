#include <iostream>
#include <chrono>
#include <thread>
using namespace std::chrono_literals;
int main() {
 auto start = std::chrono::steady_clock::now();
 auto end_time = start + 5s;
 auto task1_next = start;
 auto task2_next = start;
 auto task3_next = start;
 while (std::chrono::steady_clock::now() < end_time) {
 auto now = std::chrono::steady_clock::now();
 auto t = std::chrono::duration_cast<std::chrono::milliseconds>(now -
start).count();
 if (now >= task1_next) {
 std::cout << "[" << t << " ms] Task 1 executing\n";
 std::this_thread::sleep_for(20ms);
 task1_next += 100ms;
 }
 if (now >= task2_next) {
 std::cout << "[" << t << " ms] Task 2 executing\n";
 std::this_thread::sleep_for(40ms);
 task2_next += 250ms;
 }
 if (now >= task3_next) {
 std::cout << "[" << t << " ms] Task 3 executing\n";
 std::this_thread::sleep_for(80ms);
 task3_next += 1000ms;
 }
 std::this_thread::sleep_for(1ms);
 }
 return 0;
}