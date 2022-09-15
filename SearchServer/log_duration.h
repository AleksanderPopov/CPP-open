#pragma once

#include <chrono>
#include <iostream>

#define PROFILE_CONCAT_INTERNAL(X, Y) X##Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)
#define LOG_DURATION_STREAM(x, z) LogDuration UNIQUE_VAR_NAME_PROFILE(x, z)

class LogDuration {
public:

    using Clock = std::chrono::steady_clock;

    LogDuration(const std::string& id) : LogDuration(id, std::cerr) {
    }

    LogDuration(const std::string& id, std::ostream& out)
        : id_(id), out_(out) {
    }


    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        out_ << id_ << ": "s << duration_cast<milliseconds>(dur).count() << " ms"s << std::endl;
    }

private:
    const std::string id_;
    const Clock::time_point start_time_ = Clock::now();
    std::ostream& out_;
};
