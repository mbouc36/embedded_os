#include <iostream>
#include <chrono>
#include <thread>
using namespace std::chrono_literals;
int main() {
 auto period = 100ms;
 auto next_release = std::chrono::steady_clock::now();
 for (int i = 0; i < 20; ++i) {
 auto now = std::chrono::steady_clock::now();
 auto lateness = std::chrono::duration_cast<std::chrono::milliseconds>
(now - next_release).count();
 std::cout << "Task release " << i
 << ", lateness = " << lateness << " ms\n";
 std::this_thread::sleep_for(30ms);
 next_release += period;
 std::this_thread::sleep_until(next_release);
 }
 return 0;

}
