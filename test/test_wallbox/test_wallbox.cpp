#include <Arduino.h>
#include <unity.h>

#include "Components/Wallbox/DummyWallbox.h"
#include "Components/Wallbox/HeidelbergWallbox.h"
#include "Configuration/Constants.h"
#include "MockModbusRTU.h"

void setUp(void)
{
    MockModbus::Reset();
    HeidelbergWallbox::Instance()->Init();
}

void tearDown(void)
{
}

// --- HeidelbergWallbox ---

void test_Init_WritesConfigurationRegisters(void)
{
    // 16.0 A failsafe current, scaled by 0.1
    TEST_ASSERT_EQUAL_UINT16(160, MockModbus::GetRegister(Constants::HeidelbergRegisters::FailsafeCurrent));
    // Constants::HeidelbergWallbox::AllowStandby is false, so standby is disabled (4)
    TEST_ASSERT_EQUAL_UINT16(4, MockModbus::GetRegister(Constants::HeidelbergRegisters::DisableStandby));
    TEST_ASSERT_EQUAL_UINT16(Constants::HeidelbergWallbox::WatchdogTimeoutS, MockModbus::GetRegister(Constants::HeidelbergRegisters::WatchdogTimeout));
}

void test_Init_SeedsCurrentLimitFromRegister(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::MaximalCurrent, 120);
    HeidelbergWallbox::Instance()->Init();
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, HeidelbergWallbox::Instance()->GetChargingCurrentLimit());
}

void test_SetChargingCurrentLimit_BelowMinimum_BlocksCharging(void)
{
    TEST_ASSERT_TRUE(HeidelbergWallbox::Instance()->SetChargingCurrentLimit(5.9f));
    TEST_ASSERT_EQUAL_UINT16(0, MockModbus::GetRegister(Constants::HeidelbergRegisters::MaximalCurrent));
}

void test_SetChargingCurrentLimit_AboveMaximum_ClampsToMaximum(void)
{
    TEST_ASSERT_TRUE(HeidelbergWallbox::Instance()->SetChargingCurrentLimit(20.0f));
    TEST_ASSERT_EQUAL_UINT16(160, MockModbus::GetRegister(Constants::HeidelbergRegisters::MaximalCurrent));
}

void test_SetChargingCurrentLimit_WithinRange_WritesScaledValue(void)
{
    TEST_ASSERT_TRUE(HeidelbergWallbox::Instance()->SetChargingCurrentLimit(10.5f));
    TEST_ASSERT_EQUAL_UINT16(105, MockModbus::GetRegister(Constants::HeidelbergRegisters::MaximalCurrent));
}

void test_SetChargingCurrentLimit_WriteFails_ReturnsFalse(void)
{
    MockModbus::SetWriteSuccess(false);
    TEST_ASSERT_FALSE(HeidelbergWallbox::Instance()->SetChargingCurrentLimit(12.0f));
}

void test_GetState_MapsRegisterToVehicleState(void)
{
    auto *wallbox = HeidelbergWallbox::Instance();

    MockModbus::SetRegister(Constants::HeidelbergRegisters::ChargingState, 3);
    TEST_ASSERT_EQUAL(VehicleState::Disconnected, wallbox->GetState());

    MockModbus::SetRegister(Constants::HeidelbergRegisters::ChargingState, 4);
    TEST_ASSERT_EQUAL(VehicleState::Connected, wallbox->GetState());

    MockModbus::SetRegister(Constants::HeidelbergRegisters::ChargingState, 7);
    TEST_ASSERT_EQUAL(VehicleState::Charging, wallbox->GetState());
}

void test_GetState_ReadFails_KeepsLastKnownState(void)
{
    auto *wallbox = HeidelbergWallbox::Instance();
    MockModbus::SetRegister(Constants::HeidelbergRegisters::ChargingState, 6);
    TEST_ASSERT_EQUAL(VehicleState::Charging, wallbox->GetState());

    MockModbus::SetReadSuccess(false);
    TEST_ASSERT_EQUAL(VehicleState::Charging, wallbox->GetState());
}

void test_GetChargingCurrentLimit_ReadsAndScalesRegister(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::MaximalCurrent, 123);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.3f, HeidelbergWallbox::Instance()->GetChargingCurrentLimit());
}

void test_GetChargingCurrentLimit_ReadFails_ReturnsLastKnownValue(void)
{
    auto *wallbox = HeidelbergWallbox::Instance();
    MockModbus::SetRegister(Constants::HeidelbergRegisters::MaximalCurrent, 120);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, wallbox->GetChargingCurrentLimit());

    MockModbus::SetReadSuccess(false);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, wallbox->GetChargingCurrentLimit());
}

void test_GetEnergyMeterValue_CombinesTwoRegisters(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Energy, 1);
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Energy + 1, 0xFFFE);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 131070.0f, HeidelbergWallbox::Instance()->GetEnergyMeterValue());
}

void test_GetFailsafeCurrent_ReadsAndScalesRegister(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::FailsafeCurrent, 140);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 14.0f, HeidelbergWallbox::Instance()->GetFailsafeCurrent());
}

void test_GetChargingPower_ReadsRegister(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Power, 7360);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 7360.0f, HeidelbergWallbox::Instance()->GetChargingPower());
}

void test_GetTemperature_ReadsAndScalesRegister(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::PcbTemperature, 234);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 23.4f, HeidelbergWallbox::Instance()->GetTemperature());
}

void test_GetTemperature_ReadFails_ReturnsZero(void)
{
    MockModbus::SetReadSuccess(false);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, HeidelbergWallbox::Instance()->GetTemperature());
}

void test_GetChargingCurrents_ReadsAndScalesThreePhases(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Currents, 120);
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Currents + 1, 130);
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Currents + 2, 140);

    float c1 = 0.0f, c2 = 0.0f, c3 = 0.0f;
    TEST_ASSERT_TRUE(HeidelbergWallbox::Instance()->GetChargingCurrents(c1, c2, c3));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, c1);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 13.0f, c2);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 14.0f, c3);
}

void test_GetChargingCurrents_ReadFails_ReturnsFalse(void)
{
    MockModbus::SetReadSuccess(false);

    float c1 = 0.0f, c2 = 0.0f, c3 = 0.0f;
    TEST_ASSERT_FALSE(HeidelbergWallbox::Instance()->GetChargingCurrents(c1, c2, c3));
}

void test_GetChargingVoltages_ReadsAndScalesThreePhases(void)
{
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Voltages, 230);
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Voltages + 1, 231);
    MockModbus::SetRegister(Constants::HeidelbergRegisters::Voltages + 2, 232);

    float v1 = 0.0f, v2 = 0.0f, v3 = 0.0f;
    TEST_ASSERT_TRUE(HeidelbergWallbox::Instance()->GetChargingVoltages(v1, v2, v3));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 230.0f, v1);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 231.0f, v2);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 232.0f, v3);
}

void test_GetChargingVoltages_ReadFails_ReturnsFalse(void)
{
    MockModbus::SetReadSuccess(false);

    float v1 = 0.0f, v2 = 0.0f, v3 = 0.0f;
    TEST_ASSERT_FALSE(HeidelbergWallbox::Instance()->GetChargingVoltages(v1, v2, v3));
}

void test_SetStandbyEnabled_WritesOnlyOnStateChange(void)
{
    auto *wallbox = HeidelbergWallbox::Instance();
    // Init() already disabled standby (Constants::HeidelbergWallbox::AllowStandby == false)
    TEST_ASSERT_FALSE(wallbox->GetStandbyEnabled());

    uint32_t writesAfterInit = MockModbus::GetWriteCount(Constants::HeidelbergRegisters::DisableStandby);
    TEST_ASSERT_TRUE(wallbox->SetStandbyEnabled(false)); // no state change
    TEST_ASSERT_EQUAL(writesAfterInit, MockModbus::GetWriteCount(Constants::HeidelbergRegisters::DisableStandby));

    TEST_ASSERT_TRUE(wallbox->SetStandbyEnabled(true)); // 0 = standby allowed
    TEST_ASSERT_EQUAL_UINT16(0, MockModbus::GetRegister(Constants::HeidelbergRegisters::DisableStandby));
    TEST_ASSERT_TRUE(wallbox->GetStandbyEnabled());

    TEST_ASSERT_TRUE(wallbox->SetStandbyEnabled(false)); // 4 = standby disabled
    TEST_ASSERT_EQUAL_UINT16(4, MockModbus::GetRegister(Constants::HeidelbergRegisters::DisableStandby));
    TEST_ASSERT_FALSE(wallbox->GetStandbyEnabled());
}

void test_SetStandbyEnabled_WriteFails_KeepsLastKnownState(void)
{
    auto *wallbox = HeidelbergWallbox::Instance();
    TEST_ASSERT_FALSE(wallbox->GetStandbyEnabled());

    MockModbus::SetWriteSuccess(false);
    TEST_ASSERT_FALSE(wallbox->SetStandbyEnabled(true));
    MockModbus::SetWriteSuccess(true);

    TEST_ASSERT_FALSE(wallbox->GetStandbyEnabled());
}

void test_GetStandbyEnabled_ReadFails_ReturnsLastKnownState(void)
{
    auto *wallbox = HeidelbergWallbox::Instance();
    TEST_ASSERT_TRUE(wallbox->SetStandbyEnabled(true));
    TEST_ASSERT_TRUE(wallbox->GetStandbyEnabled());

    MockModbus::SetReadSuccess(false);
    TEST_ASSERT_TRUE(wallbox->GetStandbyEnabled());
}

// --- DummyWallbox ---

// Note: DummyWallbox is a singleton, so tests must set up the state they
// rely on instead of assuming factory defaults.

void test_DummyWallbox_DefaultState(void)
{
    auto *wallbox = DummyWallbox::Instance();
    TEST_ASSERT_FLOAT_WITHIN(0.001f, Constants::DummyWallbox::MaxChargingCurrentA, wallbox->GetChargingCurrentLimit());
    TEST_ASSERT_EQUAL(VehicleState::Charging, wallbox->GetState());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, Constants::DummyWallbox::FailSafeCurrentA, wallbox->GetFailsafeCurrent());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, Constants::DummyWallbox::TemperatureDegCel, wallbox->GetTemperature());
    TEST_ASSERT_FALSE(wallbox->GetStandbyEnabled());
}

void test_DummyWallbox_SetChargingCurrentLimit_ClampsToWallboxRange(void)
{
    auto *wallbox = DummyWallbox::Instance();
    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(20.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, Constants::HeidelbergWallbox::MaxChargingCurrentA, wallbox->GetChargingCurrentLimit());

    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(5.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, wallbox->GetChargingCurrentLimit());
}

void test_DummyWallbox_StateFollowsChargingCurrentLimit(void)
{
    auto *wallbox = DummyWallbox::Instance();
    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(12.0f));
    TEST_ASSERT_EQUAL(VehicleState::Charging, wallbox->GetState());

    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(0.0f));
    TEST_ASSERT_EQUAL(VehicleState::Connected, wallbox->GetState());
}

void test_DummyWallbox_ChargingPower_MatchesThreePhaseFormula(void)
{
    auto *wallbox = DummyWallbox::Instance();
    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(16.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 16.0f * 230.0f * 3.0f, wallbox->GetChargingPower());

    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(0.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, wallbox->GetChargingPower());
}

void test_DummyWallbox_EnergyAccumulatesOnlyWhileCharging(void)
{
    auto *wallbox = DummyWallbox::Instance();
    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(12.0f));

    float before = wallbox->GetEnergyMeterValue();
    float after = wallbox->GetEnergyMeterValue();
    TEST_ASSERT_FLOAT_WITHIN(0.001f, before + 100.0f, after);

    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(0.0f));
    float blockedBefore = wallbox->GetEnergyMeterValue();
    float blockedAfter = wallbox->GetEnergyMeterValue();
    TEST_ASSERT_FLOAT_WITHIN(0.001f, blockedBefore, blockedAfter);
}

void test_DummyWallbox_PhaseCurrentsAndVoltages(void)
{
    auto *wallbox = DummyWallbox::Instance();
    TEST_ASSERT_TRUE(wallbox->SetChargingCurrentLimit(12.0f));

    float c1 = 0.0f, c2 = 0.0f, c3 = 0.0f;
    TEST_ASSERT_TRUE(wallbox->GetChargingCurrents(c1, c2, c3));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, c1);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, c2);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, c3);

    float v1 = 0.0f, v2 = 0.0f, v3 = 0.0f;
    TEST_ASSERT_TRUE(wallbox->GetChargingVoltages(v1, v2, v3));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, Constants::DummyWallbox::ChargingVoltageV, v1);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, Constants::DummyWallbox::ChargingVoltageV, v2);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, Constants::DummyWallbox::ChargingVoltageV, v3);
}

void test_DummyWallbox_StandbyToggles(void)
{
    auto *wallbox = DummyWallbox::Instance();
    TEST_ASSERT_TRUE(wallbox->SetStandbyEnabled(true));
    TEST_ASSERT_TRUE(wallbox->GetStandbyEnabled());

    TEST_ASSERT_TRUE(wallbox->SetStandbyEnabled(false));
    TEST_ASSERT_FALSE(wallbox->GetStandbyEnabled());
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    // HeidelbergWallbox
    RUN_TEST(test_Init_WritesConfigurationRegisters);
    RUN_TEST(test_Init_SeedsCurrentLimitFromRegister);
    RUN_TEST(test_SetChargingCurrentLimit_BelowMinimum_BlocksCharging);
    RUN_TEST(test_SetChargingCurrentLimit_AboveMaximum_ClampsToMaximum);
    RUN_TEST(test_SetChargingCurrentLimit_WithinRange_WritesScaledValue);
    RUN_TEST(test_SetChargingCurrentLimit_WriteFails_ReturnsFalse);
    RUN_TEST(test_GetState_MapsRegisterToVehicleState);
    RUN_TEST(test_GetState_ReadFails_KeepsLastKnownState);
    RUN_TEST(test_GetChargingCurrentLimit_ReadsAndScalesRegister);
    RUN_TEST(test_GetChargingCurrentLimit_ReadFails_ReturnsLastKnownValue);
    RUN_TEST(test_GetEnergyMeterValue_CombinesTwoRegisters);
    RUN_TEST(test_GetFailsafeCurrent_ReadsAndScalesRegister);
    RUN_TEST(test_GetChargingPower_ReadsRegister);
    RUN_TEST(test_GetTemperature_ReadsAndScalesRegister);
    RUN_TEST(test_GetTemperature_ReadFails_ReturnsZero);
    RUN_TEST(test_GetChargingCurrents_ReadsAndScalesThreePhases);
    RUN_TEST(test_GetChargingCurrents_ReadFails_ReturnsFalse);
    RUN_TEST(test_GetChargingVoltages_ReadsAndScalesThreePhases);
    RUN_TEST(test_GetChargingVoltages_ReadFails_ReturnsFalse);
    RUN_TEST(test_SetStandbyEnabled_WritesOnlyOnStateChange);
    RUN_TEST(test_SetStandbyEnabled_WriteFails_KeepsLastKnownState);
    RUN_TEST(test_GetStandbyEnabled_ReadFails_ReturnsLastKnownState);

    // DummyWallbox
    RUN_TEST(test_DummyWallbox_DefaultState);
    RUN_TEST(test_DummyWallbox_SetChargingCurrentLimit_ClampsToWallboxRange);
    RUN_TEST(test_DummyWallbox_StateFollowsChargingCurrentLimit);
    RUN_TEST(test_DummyWallbox_ChargingPower_MatchesThreePhaseFormula);
    RUN_TEST(test_DummyWallbox_EnergyAccumulatesOnlyWhileCharging);
    RUN_TEST(test_DummyWallbox_PhaseCurrentsAndVoltages);
    RUN_TEST(test_DummyWallbox_StandbyToggles);

    return UNITY_END();
}
