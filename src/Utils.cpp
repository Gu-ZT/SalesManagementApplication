#include "Utils.h"

SnowflakeIDGenerator generator = SnowflakeIDGenerator(0b1011011001);

SnowflakeIDGenerator::SnowflakeIDGenerator(int machine_id) : machine_id_(machine_id), sequence_(0),
                                                             last_timestamp_(-1) {
    if (machine_id_ < 0 || machine_id_ >= (1 << kMachineIdBits)) {
        throw std::runtime_error("Machine ID out of range");
    }
}

long long SnowflakeIDGenerator::nextId() {
    std::lock_guard<std::mutex> lock(mutex_);

    long long timestamp = Time::current();
    if (timestamp < last_timestamp_) {
        throw std::runtime_error("Clock moved backwards. Refusing to generate id");
    }

    if (timestamp == last_timestamp_) {
        sequence_ = (sequence_ + 1) & kSequenceMask;
        if (sequence_ == 0) {
            timestamp = waitNextMillis(timestamp);
        }
    } else {
        sequence_ = 0;
    }

    last_timestamp_ = timestamp;

    return ((timestamp - kEpoch) << kTimestampLeftShift) |
           (machine_id_ << kMachineIdLeftShift) |
           sequence_;
}

long long SnowflakeIDGenerator::id() {
    return generator.nextId();
}

long long SnowflakeIDGenerator::waitNextMillis(long long int last_timestamp) {
    long long timestamp = Time::current();
    while (timestamp <= last_timestamp) {
        timestamp = Time::current();
    }
    return timestamp;
}

long long Time::current() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 将时间点转换为时间戳
    auto duration = now.time_since_epoch();

    // 转换为毫秒
    long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    return millis;
}
