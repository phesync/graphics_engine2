#pragma once

#include <chrono>

double get_time() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> seconds = now.time_since_epoch();

    return seconds.count();
}