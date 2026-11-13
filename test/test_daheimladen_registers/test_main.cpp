#include <unity.h>
#include <string>
#include "Components/Modbus/DaheimladenRegisterMap.h"

struct FakeWallbox : IWallbox
{
    int reads{0};
    bool hasEnergy{true};

    void Init() override {}
    VehicleState GetState() override { reads++; return VehicleState::Charging; }
    float GetChargingCurrentLimit() override { reads++; return 10.0f; }
    float GetEnergyMeterValue() override { reads++; return 874630.0f; } // Wh
    bool HasEnergyMeterValue() override { return hasEnergy; }
    float GetFailsafeCurrent() override { reads++; return 16.0f; }
    float GetChargingPower() override { reads++; return 2300.0f; }
    float GetTemperature() override { return 23.4f; }
    bool GetChargingCurrents(float &c1, float &c2, float &c3) override
    {
        reads++; c1 = 10.0f; c2 = 9.5f; c3 = 0.0f; return true;
    }
    bool GetChargingVoltages(float &v1, float &v2, float &v3) override
    {
        reads++; v1 = 230.0f; v2 = 231.0f; v3 = 229.5f; return true;
    }
    bool SetChargingCurrentLimit(float) override { return true; }
    bool SetStandbyEnabled(bool) override { return true; }
    bool GetStandbyEnabled() override { return false; }
};

static const char *Serial = "heidelbridge";
constexpr auto Ok = DaheimladenRegisterMap::ReadResult::Ok;
constexpr auto Unmapped = DaheimladenRegisterMap::ReadResult::UnmappedAddress;
constexpr auto Unavailable = DaheimladenRegisterMap::ReadResult::ValueUnavailable;

// Decodes a station ID the way evcc does: two characters per register, high byte first
static std::string DecodeString(const uint16_t *words, int count)
{
    std::string s;
    for (int i = 0; i < count; ++i)
    {
        for (char c : {static_cast<char>(words[i] >> 8), static_cast<char>(words[i] & 0xFF)})
            if (c != 0) s += c;
    }
    return s;
}

void setUp() {}
void tearDown() {}

// evcc without a sponsor token reads 22 registers from 32 (checkStation)
void test_evcc_station_check_block()
{
    FakeWallbox w; uint16_t out[22];
    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 32, 22, out));
    TEST_ASSERT_EQUAL_UINT16(160, out[0]); // EVSE max current 16 A
    TEST_ASSERT_EQUAL_UINT16(0, out[1]);
    TEST_ASSERT_EQUAL_UINT16(60, out[2]);  // EVSE min current 6 A
    TEST_ASSERT_EQUAL_UINT16(160, out[4]); // cable max current
    TEST_ASSERT_EQUAL_STRING("heidelbridge", DecodeString(out + 6, 16).c_str());
}

// The reads evcc and existing clients already made keep their answers
void test_single_value_reads_unchanged()
{
    FakeWallbox w; uint16_t out[16];

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 0, 1, out));
    TEST_ASSERT_EQUAL_UINT16(4, out[0]);

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 6, 5, out));
    const uint16_t currents[] = {100, 0, 95, 0, 0};
    TEST_ASSERT_EQUAL_UINT16_ARRAY(currents, out, 5);

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 12, 2, out));
    TEST_ASSERT_EQUAL_UINT16(0, out[0]);
    TEST_ASSERT_EQUAL_UINT16(2300, out[1]);

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 28, 2, out)); // 0.1 kWh, high word first
    TEST_ASSERT_EQUAL_UINT16(8746 >> 16, out[0]);
    TEST_ASSERT_EQUAL_UINT16(8746 & 0xFFFF, out[1]);

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 38, 16, out));
    TEST_ASSERT_EQUAL_STRING("heidelbridge", DecodeString(out, 16).c_str());

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 54, 16, out));
    TEST_ASSERT_EQUAL_STRING("", DecodeString(out, 16).c_str());

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 87, 1, out));
    TEST_ASSERT_EQUAL_UINT16(160, out[0]);
    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 89, 1, out));
    TEST_ASSERT_EQUAL_UINT16(60, out[0]);
    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 91, 1, out));
    TEST_ASSERT_EQUAL_UINT16(100, out[0]);

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 109, 5, out));
    const uint16_t voltages[] = {2300, 0, 2310, 0, 2295};
    TEST_ASSERT_EQUAL_UINT16_ARRAY(voltages, out, 5);
}

// A read may start inside a value and span several values
void test_partial_and_spanning_reads()
{
    FakeWallbox w; uint16_t out[8];
    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 29, 1, out));
    TEST_ASSERT_EQUAL_UINT16(8746, out[0]);

    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 87, 6, out));
    const uint16_t expected[] = {160, 0, 60, 0, 100, 0};
    TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, out, 6);
}

void test_unmapped_registers_rejected()
{
    FakeWallbox w; uint16_t out[125];
    TEST_ASSERT_TRUE(Unmapped == DaheimladenRegisterMap::Read(w, Serial, 30, 1, out));
    TEST_ASSERT_TRUE(Unmapped == DaheimladenRegisterMap::Read(w, Serial, 28, 4, out)); // runs into 30
    TEST_ASSERT_TRUE(Unmapped == DaheimladenRegisterMap::Read(w, Serial, 70, 1, out));
    TEST_ASSERT_TRUE(Unmapped == DaheimladenRegisterMap::Read(w, Serial, 65500, 125, out)); // beyond 0xFFFF
    TEST_ASSERT_EQUAL(0, w.reads); // a rejected request reads nothing from the wallbox
}

// A long block reads each value from the wallbox once
void test_each_value_read_once()
{
    FakeWallbox w; uint16_t out[8];
    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 6, 8, out));
    TEST_ASSERT_EQUAL(2, w.reads); // currents, power
    const uint16_t expected[] = {100, 0, 95, 0, 0, 0, 0, 2300};
    TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, out, 8);
}

// Before the energy counter has a baseline, reads covering it are refused instead of answered with 0
void test_energy_without_value_is_unavailable()
{
    FakeWallbox w; w.hasEnergy = false; uint16_t out[8];
    TEST_ASSERT_TRUE(Unavailable == DaheimladenRegisterMap::Read(w, Serial, 28, 2, out));
    TEST_ASSERT_TRUE(Unavailable == DaheimladenRegisterMap::Read(w, Serial, 29, 1, out)); // partly covered
    TEST_ASSERT_EQUAL(0, w.reads); // a refused request reads nothing from the wallbox
    TEST_ASSERT_TRUE(Ok == DaheimladenRegisterMap::Read(w, Serial, 12, 2, out)); // other values unaffected
    TEST_ASSERT_TRUE(Unmapped == DaheimladenRegisterMap::Read(w, Serial, 28, 4, out)); // unmapped wins
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_evcc_station_check_block);
    RUN_TEST(test_single_value_reads_unchanged);
    RUN_TEST(test_partial_and_spanning_reads);
    RUN_TEST(test_unmapped_registers_rejected);
    RUN_TEST(test_each_value_read_once);
    RUN_TEST(test_energy_without_value_is_unavailable);
    return UNITY_END();
}
