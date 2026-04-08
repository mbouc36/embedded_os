// ped_crossing_thread_timer.cpp

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

// Forward declaration
class PedCrossingController;

// ===================== State Interface =====================
class TrafficState {
public:
    virtual ~TrafficState() = default;
    virtual void enter(PedCrossingController& ctx) = 0;
    virtual void handlePedButton(PedCrossingController& ctx) = 0;
    virtual void handleTimer(PedCrossingController& ctx) = 0;
};

// ===================== Controller =====================
class PedCrossingController {
public:
    PedCrossingController();
    ~PedCrossingController();

    void setState(TrafficState& newState);
    void onPedButtonPressed();
    void onTimerExpired();

    void startYellowTimer();
    void startRedTimer();
    void startPedClearTimer();
    void returnToCarsGreen();

    void setCarsLights(const std::string& mode);
    void setPedLights(const std::string& mode);

    void runDemo();

private:
    TrafficState* currentState;

    std::mutex state_mutex;

    std::thread timer_thread;
    std::mutex timer_mutex;
    std::condition_variable timer_cv;

    bool timer_active = false;
    std::chrono::milliseconds timer_duration{0};
    std::atomic<bool> stop_flag{false};
    unsigned long timer_generation = 0;

    void timerLoop();
    void startTimer(std::chrono::milliseconds duration);
};

// ===================== States =====================
class CarsGreenState : public TrafficState {
public:
    static CarsGreenState& instance() {
        static CarsGreenState s;
        return s;
    }

    void enter(PedCrossingController& ctx) override;
    void handlePedButton(PedCrossingController& ctx) override;
    void handleTimer(PedCrossingController& ctx) override;

private:
    CarsGreenState() = default;
};

class CarsYellowState : public TrafficState {
public:
    static CarsYellowState& instance() {
        static CarsYellowState s;
        return s;
    }

    void enter(PedCrossingController& ctx) override;
    void handlePedButton(PedCrossingController& ctx) override;
    void handleTimer(PedCrossingController& ctx) override;

private:
    CarsYellowState() = default;
};

class CarsRedState : public TrafficState {
public:
    static CarsRedState& instance() {
        static CarsRedState s;
        return s;
    }

    void enter(PedCrossingController& ctx) override;
    void handlePedButton(PedCrossingController& ctx) override;
    void handleTimer(PedCrossingController& ctx) override;

private:
    CarsRedState() = default;
};

class PedClearingState : public TrafficState {
public:
    static PedClearingState& instance() {
        static PedClearingState s;
        return s;
    }

    void enter(PedCrossingController& ctx) override;
    void handlePedButton(PedCrossingController& ctx) override;
    void handleTimer(PedCrossingController& ctx) override;

private:
    PedClearingState() = default;
};

// ===================== Timer Logic =====================
void PedCrossingController::timerLoop() {
    std::unique_lock<std::mutex> lock(timer_mutex);

    while (!stop_flag.load()) {
        timer_cv.wait(lock, [this] {
            return stop_flag.load() || timer_active;
        });

        if (stop_flag.load()) break;

        auto duration = timer_duration;
        auto my_generation = timer_generation;

        lock.unlock();
        std::this_thread::sleep_for(duration);
        lock.lock();

        if (stop_flag.load()) break;
        if (my_generation != timer_generation) continue;

        timer_active = false;

        lock.unlock();
        onTimerExpired();
        lock.lock();
    }
}

void PedCrossingController::startTimer(std::chrono::milliseconds duration) {
    std::lock_guard<std::mutex> lock(timer_mutex);
    timer_duration = duration;
    timer_active = true;
    ++timer_generation;
    timer_cv.notify_one();
}

// ===================== Controller Impl =====================
PedCrossingController::PedCrossingController()
    : currentState(&CarsGreenState::instance()) {
    currentState->enter(*this);
    timer_thread = std::thread(&PedCrossingController::timerLoop, this);
}

PedCrossingController::~PedCrossingController() {
    {
        std::lock_guard<std::mutex> lock(timer_mutex);
        stop_flag.store(true);
        timer_active = false;
        timer_cv.notify_all();
    }

    if (timer_thread.joinable()) {
        timer_thread.join();
    }
}

void PedCrossingController::setState(TrafficState& newState) {
    currentState = &newState;
    currentState->enter(*this);
}

void PedCrossingController::onPedButtonPressed() {
    std::lock_guard<std::mutex> lock(state_mutex);
    std::cout << "[Controller] Event: pedButtonPressed\n";
    currentState->handlePedButton(*this);
}

void PedCrossingController::onTimerExpired() {
    std::lock_guard<std::mutex> lock(state_mutex);
    std::cout << "[Controller] Event: timerExpired\n";
    currentState->handleTimer(*this);
}

// ===================== Controller Actions =====================
void PedCrossingController::startYellowTimer() {
    std::cout << "[Controller] Action: startYellowTimer() (3 seconds)\n";
    startTimer(std::chrono::seconds(3));
}

void PedCrossingController::startRedTimer() {
    std::cout << "[Controller] Action: startRedTimer() (5 seconds)\n";
    startTimer(std::chrono::seconds(5));
}

void PedCrossingController::startPedClearTimer() {
    std::cout << "[Controller] Action: startPedClearTimer() (3 seconds)\n";
    startTimer(std::chrono::seconds(3));
}

void PedCrossingController::returnToCarsGreen() {
    std::cout << "[Controller] Action: returnToCarsGreen()\n";
    setState(CarsGreenState::instance());
    std::cout << "\nEnter command (b/q): " << std::flush;
}

void PedCrossingController::setCarsLights(const std::string& mode) {
    std::cout << "[Controller] Cars lights: " << mode << "\n";
}

void PedCrossingController::setPedLights(const std::string& mode) {
    std::cout << "[Controller] Pedestrian lights: " << mode << "\n";
}

// ===================== Demo =====================
void PedCrossingController::runDemo() {
    std::cout << "==== Pedestrian Crossing State Machine Demo (Real Timer) ====\n";
    std::cout << "Commands:\n";
    std::cout << " b - pedButtonPressed\n";
    std::cout << " q - quit\n";

    char cmd;

    while (true) {
        std::cout << "\nEnter command (b/q): ";
        if (!(std::cin >> cmd)) break;

        if (cmd == 'q') {
            std::cout << "Exiting demo.\n";
            break;
        } else if (cmd == 'b') {
            onPedButtonPressed();
        } else {
            std::cout << "Unknown command.\n";
        }
    }
}

// ===================== State Implementations =====================
void CarsGreenState::enter(PedCrossingController& ctx) {
    std::cout << "\n[State] Entering CarsGreen\n";
    ctx.setCarsLights("GREEN");
    ctx.setPedLights("RED");
}

void CarsGreenState::handlePedButton(PedCrossingController& ctx) {
    std::cout << "[CarsGreen] pedButtonPressed: going to CarsYellow\n";
    ctx.setState(CarsYellowState::instance());
    ctx.startYellowTimer();
}

void CarsGreenState::handleTimer(PedCrossingController&) {
    std::cout << "[CarsGreen] timerExpired: ignored\n";
}

void CarsYellowState::enter(PedCrossingController& ctx) {
    std::cout << "\n[State] Entering CarsYellow\n";
    ctx.setCarsLights("YELLOW");
    ctx.setPedLights("RED");
}

void CarsYellowState::handlePedButton(PedCrossingController&) {
    std::cout << "[CarsYellow] pedButtonPressed: ignored\n";
}

void CarsYellowState::handleTimer(PedCrossingController& ctx) {
    std::cout << "[CarsYellow] timerExpired: going to CarsRed\n";
    ctx.setState(CarsRedState::instance());
    ctx.startRedTimer();
}

void CarsRedState::enter(PedCrossingController& ctx) {
    std::cout << "\n[State] Entering CarsRed\n";
    ctx.setCarsLights("RED");
    ctx.setPedLights("GREEN");
}

void CarsRedState::handlePedButton(PedCrossingController&) {
    std::cout << "[CarsRed] pedButtonPressed: ignored\n";
}

void CarsRedState::handleTimer(PedCrossingController& ctx) {
    std::cout << "[CarsRed] timerExpired: going to PedClearing\n";
    ctx.setState(PedClearingState::instance());
    ctx.startPedClearTimer();
}

void PedClearingState::enter(PedCrossingController& ctx) {
    std::cout << "\n[State] Entering PedClearing\n";
    ctx.setCarsLights("RED");
    ctx.setPedLights("FLASHING");
}

void PedClearingState::handlePedButton(PedCrossingController&) {
    std::cout << "[PedClearing] pedButtonPressed: ignored\n";
}

void PedClearingState::handleTimer(PedCrossingController& ctx) {
    std::cout << "[PedClearing] timerExpired: returning to CarsGreen\n";
    ctx.returnToCarsGreen();
}

// ===================== Main =====================
int main() {
    PedCrossingController controller;
    controller.runDemo();
    return 0;
}