#include "EnergyCounter.h"

EnergyCounter::EnergyCounter(Persistence &persistence)
    : EnergyCounter(persistence, Config{})
{
}

EnergyCounter::EnergyCounter(Persistence &persistence, const Config &config)
    : mPersistence(persistence), mConfig(config)
{
}

void EnergyCounter::Init()
{
    int64_t offsetWh = 0;
    uint32_t publishedWh = 0;
    if (mPersistence.Load(offsetWh, publishedWh))
    {
        mOffsetWh = offsetWh;
        mPublishedWh = publishedWh;
        mHwmWh = publishedWh;
        mHasPublished = true;
    }
}

uint32_t EnergyCounter::ClampToU32(int64_t value)
{
    if (value < 0)
    {
        return 0;
    }
    if (value > static_cast<int64_t>(UINT32_MAX))
    {
        return UINT32_MAX;
    }
    return static_cast<uint32_t>(value);
}

// A register may only rise, and no faster than the wallbox can deliver
bool EnergyCounter::IsPlausibleRise(uint32_t fromWh, uint32_t toWh, uint32_t elapsedMs) const
{
    if (toWh < fromWh)
    {
        return false;
    }
    const uint64_t maxRiseWh = (static_cast<uint64_t>(mConfig.maxPowerW) * elapsedMs * 5) / (3600ULL * 1000ULL * 4) // +25 %
                               + mConfig.riseToleranceWh;
    return static_cast<uint64_t>(toWh - fromWh) <= maxRiseWh;
}

void EnergyCounter::Update(uint32_t rawWh, uint32_t nowMs)
{
    if (!mHasAcceptedRaw)
    {
        // Nothing to compare against yet, so the first value has to prove itself over the stable window
        TrackPending(rawWh, nowMs);
        return;
    }

    if (IsPlausibleRise(mLastRawWh, rawWh, nowMs - mLastRawMs))
    {
        mPendingActive = false;
        Accept(rawWh, nowMs);
        return;
    }

    TrackPending(rawWh, nowMs);
}

// Continues the counter with a raw value that is consistent with the last one
void EnergyCounter::Accept(uint32_t rawWh, uint32_t nowMs, bool persistNow)
{
    mLastRawWh = rawWh;
    mLastRawMs = nowMs;
    mHasAcceptedRaw = true;

    const uint32_t candidate = ClampToU32(static_cast<int64_t>(rawWh) + mOffsetWh);
    if (candidate > mHwmWh)
    {
        mHwmWh = candidate;
    }
    PersistIfDue(nowMs, persistNow);
}

// Collects a deviating value until it has been stable for the configured window, then bridges the jump
void EnergyCounter::TrackPending(uint32_t rawWh, uint32_t nowMs)
{
    const bool continues = mPendingActive && IsPlausibleRise(mPendingLastRawWh, rawWh, nowMs - mPendingLastMs);
    if (!continues)
    {
        mPendingActive = true;
        mPendingFirstRawWh = rawWh;
        mPendingStartMs = nowMs;
        mPendingSamples = 0;
    }
    mPendingLastRawWh = rawWh;
    mPendingLastMs = nowMs;
    if (mPendingSamples < UINT8_MAX)
    {
        mPendingSamples++;
    }

    if (mPendingSamples >= mConfig.stableMinSamples && (nowMs - mPendingStartMs) >= mConfig.stableWindowMs)
    {
        CommitPending(nowMs);
    }
}

void EnergyCounter::CommitPending(uint32_t nowMs)
{
    mPendingActive = false;

    if (!mHasAcceptedRaw && !mHasPublished)
    {
        // Nothing persisted yet: this value is the baseline
        Rebase(mPendingFirstRawWh, 0);
    }
    else if (!mHasAcceptedRaw && static_cast<int64_t>(mPendingFirstRawWh) + mOffsetWh >= static_cast<int64_t>(mHwmWh))
    {
        // Register moved on while the bridge was down: that energy counts
    }
    else
    {
        // Register jumped (wallbox lost power, bogus reading, replaced wallbox): continue seamlessly from the hwm
        Rebase(mPendingFirstRawWh, static_cast<int64_t>(mHwmWh) - mPendingFirstRawWh);
    }

    // Energy charged during the window counts; persist right away so the stored offset/hwm pair is fresh
    Accept(mPendingLastRawWh, nowMs, true);
}

void EnergyCounter::Rebase(uint32_t rawWh, int64_t offsetWh)
{
    if (offsetWh != mOffsetWh || mOffsetDirty)
    {
        mOffsetWh = offsetWh;
        mOffsetDirty = !mPersistence.SaveOffset(mOffsetWh);
    }
    mLastRawWh = rawWh;
    mHasAcceptedRaw = true;
}

// Persists (and thereby publishes) the hwm once it has grown by a full step, rate-limited to bound flash wear.
// Offset first: a persisted hwm must never be paired with a stale offset.
void EnergyCounter::PersistIfDue(uint32_t nowMs, bool persistNow)
{
    if (mOffsetDirty)
    {
        mOffsetDirty = !mPersistence.SaveOffset(mOffsetWh);
        if (mOffsetDirty)
        {
            return;
        }
    }

    const bool grown = mHwmWh >= mPublishedWh && (mHwmWh - mPublishedWh) >= mConfig.persistStepWh;
    const bool rateOk = persistNow || !mHasPersistTime || (nowMs - mLastPersistMs) >= mConfig.minPersistIntervalMs;
    const bool due = !mHasPublished || (grown && rateOk);
    if (due && mPersistence.SavePublished(mHwmWh))
    {
        mPublishedWh = mHwmWh;
        mHasPublished = true;
        mLastPersistMs = nowMs;
        mHasPersistTime = true;
    }
}

bool EnergyCounter::SetCounter(uint32_t targetWh)
{
    if (!mHasAcceptedRaw)
    {
        return false;
    }
    mPendingActive = false;

    const int64_t offsetWh = static_cast<int64_t>(targetWh) - mLastRawWh;
    if (!mPersistence.SaveOffset(offsetWh))
    {
        return false;
    }
    mOffsetWh = offsetWh;
    mOffsetDirty = false;

    if (!mPersistence.SavePublished(targetWh))
    {
        return false;
    }
    mHwmWh = targetWh;
    mPublishedWh = targetWh;
    mHasPublished = true;
    return true;
}
