#pragma once

#include <cstdint>

// Monotonic energy counter on top of a wallbox register that may jump: publishes raw + offset, bridges every
// implausible jump of the register with the offset and only ever publishes persisted values.
// Pure C++ without Arduino dependencies so it can be unit-tested on the host. Not thread-safe.
class EnergyCounter
{
public:
    struct Persistence
    {
        virtual ~Persistence() = default;
        virtual bool Load(int64_t &offsetWh, uint32_t &publishedWh) = 0; // false if nothing is stored yet
        virtual bool SaveOffset(int64_t offsetWh) = 0;
        virtual bool SavePublished(uint32_t publishedWh) = 0;
    };

    struct Config
    {
        uint32_t maxPowerW{11000};            // plausibility limit: 3 x 16 A x 230 V
        uint32_t riseToleranceWh{100};        // slack on top of maxPowerW x elapsed time
        uint32_t stableWindowMs{30000};       // a deviating value must hold this long ...
        uint8_t stableMinSamples{3};          // ... and be seen this often before it is trusted
        uint32_t persistStepWh{10};           // publish/persist granularity
        uint32_t minPersistIntervalMs{30000}; // flash wear cap while charging
    };

    explicit EnergyCounter(Persistence &persistence);
    EnergyCounter(Persistence &persistence, const Config &config);

    // Loads offset and published value from persistence
    void Init();

    // Feeds one successfully read raw register value, nowMs is millis()
    void Update(uint32_t rawWh, uint32_t nowMs);

    // Returns true once a value (persisted or freshly read) is available
    bool HasValue() const { return mHasPublished; }

    // Returns the published, persisted counter value
    uint32_t GetPublishedWh() const { return mPublishedWh; }

    // Rebases the counter so that it reads targetWh from now on, persisted before returning
    bool SetCounter(uint32_t targetWh);

    // Diagnostics
    int64_t GetOffsetWh() const { return mOffsetWh; }
    uint32_t GetLastRawWh() const { return mLastRawWh; }
    bool HasAcceptedRaw() const { return mHasAcceptedRaw; }
    bool IsPending() const { return mPendingActive; }

private:
    bool IsPlausibleRise(uint32_t fromWh, uint32_t toWh, uint32_t elapsedMs) const;
    void Accept(uint32_t rawWh, uint32_t nowMs, bool persistNow = false);
    void TrackPending(uint32_t rawWh, uint32_t nowMs);
    void CommitPending(uint32_t nowMs);
    void Rebase(uint32_t rawWh, int64_t offsetWh);
    void PersistIfDue(uint32_t nowMs, bool persistNow);
    static uint32_t ClampToU32(int64_t value);

    Persistence &mPersistence;
    Config mConfig;

    int64_t mOffsetWh{0};
    bool mOffsetDirty{false};

    uint32_t mHwmWh{0};       // raw + offset of the last accepted read
    uint32_t mPublishedWh{0}; // last successfully persisted hwm
    bool mHasPublished{false};
    uint32_t mLastPersistMs{0};
    bool mHasPersistTime{false};

    bool mHasAcceptedRaw{false};
    uint32_t mLastRawWh{0};
    uint32_t mLastRawMs{0};

    bool mPendingActive{false};
    uint32_t mPendingFirstRawWh{0};
    uint32_t mPendingLastRawWh{0};
    uint32_t mPendingStartMs{0};
    uint32_t mPendingLastMs{0};
    uint8_t mPendingSamples{0};
};
