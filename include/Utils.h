#ifndef APPLICATION_UTILS_H
#define APPLICATION_UTILS_H

#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
#include <chrono>

/**
 * 雪花 ID 生成器
 *
 * @author Gugle
 */
class SnowflakeIDGenerator {
public:
    explicit SnowflakeIDGenerator(int machine_id);

    /**
     * @return 生成的雪花 ID
     */
    long long nextId();

    /**
     * @return 以默认生成器生成的雪花 ID
     */
    static long long id();

private:
    int machine_id_;
    long long sequence_;
    long long last_timestamp_;
    std::mutex mutex_;

    static constexpr long long kEpoch = 1577836800000; // Custom epoch (2020-01-01T00:00:00Z)
    static constexpr int kMachineIdBits = 10;
    static constexpr int kSequenceBits = 12;
    static constexpr int kMachineIdLeftShift = kSequenceBits;
    static constexpr int kTimestampLeftShift = kSequenceBits + kMachineIdBits;
    static constexpr int kSequenceMask = (1 << kSequenceBits) - 1;

    static long long waitNextMillis(long long last_timestamp);
};

/**
 * 时间实用程序
 *
 * @author Gugle
 */
class Time {
public:
    /**
     * @return 当前系统时间（单位ms，UNIX时间戳）
     */
    static long long current();
};

#endif // APPLICATION_UTILS_H
