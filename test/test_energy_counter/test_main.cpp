#include <unity.h>
#include <vector>
#include "Components/Wallbox/EnergyCounter.h"

// Fake NVS: records every write, can be told to fail.
struct FakePersistence : EnergyCounter::Persistence
{
    bool hasData{false};
    int64_t offsetWh{0};
    uint32_t publishedWh{0};
    bool failWrites{false};
    int offsetWrites{0};
    int publishedWrites{0};

    bool Load(int64_t &o, uint32_t &p) override
    {
        if (!hasData) return false;
        o = offsetWh; p = publishedWh; return true;
    }
    bool SaveOffset(int64_t o) override
    {
        if (failWrites) return false;
        offsetWh = o; hasData = true; offsetWrites++; return true;
    }
    bool SavePublished(uint32_t p) override
    {
        if (failWrites) return false;
        publishedWh = p; hasData = true; publishedWrites++; return true;
    }
};

constexpr uint32_t POLL = 5000; // ms between reads, like the firmware poller

// Most tests look at the counter logic and want every step persisted at once.
static EnergyCounter::Config noRateLimit()
{
    EnergyCounter::Config c;
    c.minPersistIntervalMs = 0;
    return c;
}
static const EnergyCounter::Config NO_RATE_LIMIT = noRateLimit();
constexpr uint32_t kWh = 1000;

// Feeds `count` reads of `raw` starting at t, POLL apart. Returns the time after the last read.
static uint32_t feed(EnergyCounter &c, uint32_t raw, uint32_t t, int count)
{
    for (int i = 0; i < count; ++i) { c.Update(raw, t); t += POLL; }
    return t;
}

// Boots a counter with empty NVS and settles it on `raw`.
static uint32_t boot(EnergyCounter &c, uint32_t raw, uint32_t t = 0)
{
    c.Init();
    return feed(c, raw, t, 7); // 7 samples = 30 s window
}

void test_empty_nvs_first_value_becomes_baseline()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    c.Init();
    TEST_ASSERT_FALSE(c.HasValue());
    uint32_t t = feed(c, 776 * kWh, 0, 6);          // 25 s: still pending
    TEST_ASSERT_FALSE(c.HasValue());
    c.Update(776 * kWh, t);                          // 30 s
    TEST_ASSERT_TRUE(c.HasValue());
    TEST_ASSERT_EQUAL_UINT32(776 * kWh, c.GetPublishedWh());
    TEST_ASSERT_EQUAL_INT64(0, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(776 * kWh, p.publishedWh);
}

void test_normal_charging_follows_register_in_steps()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    int writes = p.publishedWrites;
    c.Update(776005, t); t += POLL;                  // +5 Wh: not persisted yet
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    TEST_ASSERT_EQUAL(writes, p.publishedWrites);
    c.Update(776012, t); t += POLL;                  // +12 Wh: persisted
    TEST_ASSERT_EQUAL_UINT32(776012, c.GetPublishedWh());
    TEST_ASSERT_EQUAL(writes + 1, p.publishedWrites);
    for (int i = 1; i <= 100; ++i) { c.Update(776012 + i * 15, t); t += POLL; }
    TEST_ASSERT_EQUAL_UINT32(776012 + 1500, c.GetPublishedWh());
}

void test_bridge_reboot_only_keeps_value_and_counts_energy_charged_meanwhile()
{
    FakePersistence p; { EnergyCounter c(p, NO_RATE_LIMIT); boot(c, 776000); }
    // bridge was down for a while, wallbox charged 80 Wh
    EnergyCounter c(p, NO_RATE_LIMIT);
    c.Init();
    TEST_ASSERT_TRUE(c.HasValue());
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    uint32_t t = feed(c, 776080, 1000, 7);
    TEST_ASSERT_EQUAL_UINT32(776080, c.GetPublishedWh());
    TEST_ASSERT_EQUAL_INT64(0, c.GetOffsetWh());
    (void)t;
}

void test_power_cut_register_falls_back_is_bridged()
{
    FakePersistence p; { EnergyCounter c(p, NO_RATE_LIMIT); boot(c, 776000); }
    EnergyCounter c(p, NO_RATE_LIMIT); c.Init();
    uint32_t t = feed(c, 700000, 0, 6);
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh()); // pending: old value stays
    t = feed(c, 700000, t, 1);
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    TEST_ASSERT_EQUAL_INT64(76000, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_INT64(76000, p.offsetWh);
    c.Update(700020, t);                              // charging continues
    TEST_ASSERT_EQUAL_UINT32(776020, c.GetPublishedWh());
}

void test_power_cut_while_charging_loses_at_most_one_step()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    c.Update(776017, t);                              // persisted 776017? 17 >= 10 -> yes
    TEST_ASSERT_EQUAL_UINT32(776017, c.GetPublishedWh());
    c.Update(776024, t + POLL);                       // +7 Wh: RAM only
    TEST_ASSERT_EQUAL_UINT32(776017, c.GetPublishedWh());
    // power cut: bridge reboots, wallbox falls back
    EnergyCounter c2(p, NO_RATE_LIMIT); c2.Init();
    TEST_ASSERT_EQUAL_UINT32(776017, c2.GetPublishedWh());
    feed(c2, 700000, 0, 7);
    TEST_ASSERT_EQUAL_UINT32(776017, c2.GetPublishedWh()); // monotone, 7 Wh lost
}

void test_charging_during_confirmation_window_counts()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    // wallbox alone lost power (reads failed meanwhile), comes back low and charging
    t += 60000;
    for (int i = 0; i < 7; ++i) { c.Update(700000 + i * 15, t); t += POLL; }
    TEST_ASSERT_EQUAL_INT64(76000, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(776090, c.GetPublishedWh());
}

void test_short_dip_is_ignored()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    c.Update(700000, t); t += POLL;
    c.Update(700000, t); t += POLL;
    TEST_ASSERT_TRUE(c.IsPending());
    c.Update(776010, t); t += POLL;
    TEST_ASSERT_FALSE(c.IsPending());
    TEST_ASSERT_EQUAL_INT64(0, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(776010, c.GetPublishedWh());
}

void test_second_power_cut_before_wallbox_saved_again()
{
    FakePersistence p; { EnergyCounter c(p, NO_RATE_LIMIT); boot(c, 776000); }
    { EnergyCounter c(p, NO_RATE_LIMIT); c.Init(); uint32_t t = feed(c, 700000, 0, 7); c.Update(704000, t + 3600000); } // 4 kWh in an hour
    TEST_ASSERT_EQUAL_UINT32(780000, p.publishedWh);
    EnergyCounter c(p, NO_RATE_LIMIT); c.Init();
    feed(c, 700000, 0, 7);
    TEST_ASSERT_EQUAL_INT64(80000, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(780000, c.GetPublishedWh());
}

void test_stale_boot_value_followed_by_real_value_is_not_double_counted()
{
    FakePersistence p; { EnergyCounter c(p, NO_RATE_LIMIT); boot(c, 776000); }
    EnergyCounter c(p, NO_RATE_LIMIT); c.Init();
    uint32_t t = feed(c, 700000, 0, 7);              // stale value long enough to be trusted
    TEST_ASSERT_EQUAL_INT64(76000, c.GetOffsetWh());
    t = feed(c, 776000, t, 7);                        // wallbox now reports the real value
    TEST_ASSERT_EQUAL_INT64(0, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    c.Update(776020, t);
    TEST_ASSERT_EQUAL_UINT32(776020, c.GetPublishedWh());
}

void test_transient_zero_at_wallbox_boot()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    c.Update(0, t); t += POLL;
    c.Update(776000, t); t += POLL;
    TEST_ASSERT_EQUAL_INT64(0, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
}

void test_wallbox_replaced_with_higher_counter_continues_seamlessly()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    t = feed(c, 5000000, t, 7);
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    TEST_ASSERT_EQUAL_INT64(776000 - 5000000, c.GetOffsetWh());
    c.Update(5000030, t);
    TEST_ASSERT_EQUAL_UINT32(776030, c.GetPublishedWh());
}

void test_wallbox_replaced_with_zero_counter()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    t = feed(c, 0, t, 7);
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    c.Update(10, t);
    TEST_ASSERT_EQUAL_UINT32(776010, c.GetPublishedWh());
}

void test_single_bogus_high_reading_is_ignored()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    c.Update(900000, t); t += POLL;
    c.Update(776000, t); t += POLL;
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    TEST_ASSERT_EQUAL_INT64(0, c.GetOffsetWh());
}

void test_inconsistent_pending_samples_restart_window()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    c.Update(700000, t); t += POLL;
    c.Update(700050, t); t += POLL;
    c.Update(1200000, t); t += POLL;                  // implausible even within the pending track
    TEST_ASSERT_TRUE(c.IsPending());
    TEST_ASSERT_EQUAL_INT64(0, c.GetOffsetWh());
    t = feed(c, 1200000, t, 7);
    TEST_ASSERT_EQUAL_INT64(776000 - 1200000, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
}

void test_long_read_outage_accepts_large_rise()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    t += 3600000;                                     // one hour without successful reads
    c.Update(776000 + 10000, t);                      // 10 kWh at 11 kW is plausible
    TEST_ASSERT_FALSE(c.IsPending());
    TEST_ASSERT_EQUAL_UINT32(786000, c.GetPublishedWh());
}

void test_millis_wraparound()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000, 0xFFFFFFFFu - 20000);
    c.Update(776010, t);                              // t has wrapped past zero
    TEST_ASSERT_FALSE(c.IsPending());
    TEST_ASSERT_EQUAL_UINT32(776010, c.GetPublishedWh());
}

void test_failed_nvs_writes_are_retried_and_value_stays_monotone()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    p.failWrites = true;
    c.Update(776050, t); t += POLL;
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh()); // not published: not persisted
    t = feed(c, 700000, t, 7);                        // regression while NVS is broken
    TEST_ASSERT_EQUAL_INT64(76050, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_INT64(0, p.offsetWh);
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    p.failWrites = false;
    c.Update(700000, t);                              // next read persists offset then hwm
    TEST_ASSERT_EQUAL_INT64(76050, p.offsetWh);
    TEST_ASSERT_EQUAL_UINT32(776050, p.publishedWh);
    TEST_ASSERT_EQUAL_UINT32(776050, c.GetPublishedWh());
}

void test_offset_is_persisted_before_published()
{
    FakePersistence p; { EnergyCounter c(p, NO_RATE_LIMIT); boot(c, 776000); }
    EnergyCounter c(p, NO_RATE_LIMIT); c.Init();
    int offsetWritesBefore = p.offsetWrites;
    feed(c, 700000, 0, 7);
    TEST_ASSERT_EQUAL(offsetWritesBefore + 1, p.offsetWrites);
    // published value written after the regression equals the pre-regression hwm
    TEST_ASSERT_EQUAL_UINT32(776000, p.publishedWh);
}

void test_set_counter_rebases_and_persists()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    TEST_ASSERT_TRUE(c.SetCounter(500000));
    TEST_ASSERT_EQUAL_UINT32(500000, c.GetPublishedWh());
    TEST_ASSERT_EQUAL_INT64(-276000, p.offsetWh);
    TEST_ASSERT_EQUAL_UINT32(500000, p.publishedWh);
    c.Update(776020, t);
    TEST_ASSERT_EQUAL_UINT32(500020, c.GetPublishedWh());
    // survives a reboot
    EnergyCounter c2(p, NO_RATE_LIMIT); c2.Init();
    TEST_ASSERT_EQUAL_UINT32(500020, c2.GetPublishedWh());
    feed(c2, 776020, 0, 7);
    TEST_ASSERT_EQUAL_UINT32(500020, c2.GetPublishedWh());
}

void test_set_counter_requires_accepted_raw()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    c.Init();
    c.Update(776000, 0);
    TEST_ASSERT_FALSE(c.SetCounter(1));
}

void test_negative_offset_never_wraps()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    TEST_ASSERT_TRUE(c.SetCounter(100));
    // register falls below what the offset can absorb: clamp to 0, hwm stays
    t = feed(c, 5, t, 7);
    TEST_ASSERT_EQUAL_UINT32(100, c.GetPublishedWh());
    c.Update(25, t);
    TEST_ASSERT_EQUAL_UINT32(120, c.GetPublishedWh());
}

void test_regression_bridged_with_ram_hwm_when_bridge_stays_up()
{
    FakePersistence p; EnergyCounter c(p, NO_RATE_LIMIT);
    uint32_t t = boot(c, 776000);
    c.Update(776008, t); t += POLL;                   // 8 Wh in RAM only
    t += 120000;                                      // wallbox off for two minutes, reads failed
    t = feed(c, 700000, t, 7);
    TEST_ASSERT_EQUAL_INT64(76008, c.GetOffsetWh());
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh()); // 8 Wh still below the persist step
    c.Update(700002, t);
    TEST_ASSERT_EQUAL_UINT32(776010, c.GetPublishedWh()); // nothing lost
}

// --- flash wear cap (default config: 10 Wh step, one write per 30 s) ---

void test_persist_is_rate_limited_while_charging()
{
    FakePersistence p; EnergyCounter c(p);
    uint32_t t = boot(c, 776000);                      // baseline persisted at t-5s
    int writes = p.publishedWrites;
    // 22 kW: +30 Wh per 5 s poll
    uint32_t raw = 776000;
    for (int i = 1; i <= 12; ++i)                      // 60 s
    {
        raw += 30; c.Update(raw, t); t += POLL;
    }
    TEST_ASSERT_EQUAL(writes + 2, p.publishedWrites);  // at +30 s and +60 s, not 12 times
    TEST_ASSERT_TRUE(c.GetPublishedWh() <= raw);
    TEST_ASSERT_TRUE(raw - c.GetPublishedWh() <= 30 * 6); // lags at most one interval
}

void test_rebase_persists_immediately_despite_rate_limit()
{
    FakePersistence p; EnergyCounter c(p);
    uint32_t t = boot(c, 776000);
    c.Update(776030, t); t += POLL;                    // within 30 s of the baseline: RAM only
    TEST_ASSERT_EQUAL_UINT32(776000, c.GetPublishedWh());
    for (int i = 0; i < 7; ++i) { c.Update(700000 + i * 15, t); t += POLL; } // regression, charging
    TEST_ASSERT_EQUAL_INT64(76030, p.offsetWh);
    TEST_ASSERT_EQUAL_UINT32(776120, p.publishedWh);   // 776030 + 6 x 15, written at the commit
}

void test_first_value_is_not_rate_limited()
{
    FakePersistence p; EnergyCounter c(p);
    c.Init();
    feed(c, 776000, 0, 7);
    TEST_ASSERT_TRUE(c.HasValue());
    TEST_ASSERT_EQUAL_UINT32(776000, p.publishedWh);
}

int main(int, char **)
{
    UNITY_BEGIN();
    RUN_TEST(test_empty_nvs_first_value_becomes_baseline);
    RUN_TEST(test_normal_charging_follows_register_in_steps);
    RUN_TEST(test_bridge_reboot_only_keeps_value_and_counts_energy_charged_meanwhile);
    RUN_TEST(test_power_cut_register_falls_back_is_bridged);
    RUN_TEST(test_power_cut_while_charging_loses_at_most_one_step);
    RUN_TEST(test_charging_during_confirmation_window_counts);
    RUN_TEST(test_short_dip_is_ignored);
    RUN_TEST(test_second_power_cut_before_wallbox_saved_again);
    RUN_TEST(test_stale_boot_value_followed_by_real_value_is_not_double_counted);
    RUN_TEST(test_transient_zero_at_wallbox_boot);
    RUN_TEST(test_wallbox_replaced_with_higher_counter_continues_seamlessly);
    RUN_TEST(test_wallbox_replaced_with_zero_counter);
    RUN_TEST(test_single_bogus_high_reading_is_ignored);
    RUN_TEST(test_inconsistent_pending_samples_restart_window);
    RUN_TEST(test_long_read_outage_accepts_large_rise);
    RUN_TEST(test_millis_wraparound);
    RUN_TEST(test_failed_nvs_writes_are_retried_and_value_stays_monotone);
    RUN_TEST(test_offset_is_persisted_before_published);
    RUN_TEST(test_set_counter_rebases_and_persists);
    RUN_TEST(test_set_counter_requires_accepted_raw);
    RUN_TEST(test_negative_offset_never_wraps);
    RUN_TEST(test_regression_bridged_with_ram_hwm_when_bridge_stays_up);
    RUN_TEST(test_persist_is_rate_limited_while_charging);
    RUN_TEST(test_rebase_persists_immediately_despite_rate_limit);
    RUN_TEST(test_first_value_is_not_rate_limited);
    return UNITY_END();
}
