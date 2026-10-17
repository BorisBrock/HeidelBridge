#include <cstring>
#include <unity.h>

#include "Components/MQTT/MqttCommandParser.h"
#include "Utils/PendingCommand.h"

void setUp(void)
{
}

void tearDown(void)
{
}

namespace
{
    bool ParseCurrent(const char *payload, float &current)
    {
        return MqttCommandParser::ParseChargingCurrent(payload, strlen(payload), current);
    }

    bool ParseStandby(const char *payload, bool &standby)
    {
        return MqttCommandParser::ParseStandby(payload, strlen(payload), standby);
    }
}

// --- ParseChargingCurrent ---

void test_ParseChargingCurrent_AcceptsPlainNumbers(void)
{
    float current = -1.0f;
    TEST_ASSERT_TRUE(ParseCurrent("0", current));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, current);
    TEST_ASSERT_TRUE(ParseCurrent("16", current));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 16.0f, current);
    TEST_ASSERT_TRUE(ParseCurrent("6.5", current));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 6.5f, current);
    TEST_ASSERT_TRUE(ParseCurrent(".5", current));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, current);
    TEST_ASSERT_TRUE(ParseCurrent("10.", current));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, current);
}

void test_ParseChargingCurrent_TrimsWhitespace(void)
{
    float current = 0.0f;
    TEST_ASSERT_TRUE(ParseCurrent("  12.3\r\n", current));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.3f, current);
}

void test_ParseChargingCurrent_RejectsGarbage(void)
{
    const char *invalid[] = {"", "   ", "abc", "ON", "12A", "1.2.3", ".", "-6", "+6",
                             "nan", "NaN", "inf", "-inf", "0x10", "1e1", "{\"value\":6}", "6 7"};
    for (const char *payload : invalid)
    {
        float current = 42.0f;
        TEST_ASSERT_FALSE_MESSAGE(ParseCurrent(payload, current), payload);
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, 42.0f, current, payload); // untouched on failure
    }
}

void test_ParseChargingCurrent_RejectsTooLongPayload(void)
{
    float current = 0.0f;
    TEST_ASSERT_FALSE(ParseCurrent("00000000000000006", current));
}

void test_ParseChargingCurrent_UsesLengthNotTerminator(void)
{
    // MQTT payloads are not null-terminated: only the first len bytes count
    const char payload[] = {'1', '0', '9', '9'};
    float current = 0.0f;
    TEST_ASSERT_TRUE(MqttCommandParser::ParseChargingCurrent(payload, 2, current));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, current);
}

void test_ParseChargingCurrent_NullPayload_IsRejected(void)
{
    float current = 0.0f;
    TEST_ASSERT_FALSE(MqttCommandParser::ParseChargingCurrent(nullptr, 0, current));
}

// --- ParseStandby ---

void test_ParseStandby_AcceptsOnOffCaseInsensitive(void)
{
    bool standby = false;
    TEST_ASSERT_TRUE(ParseStandby("ON", standby));
    TEST_ASSERT_TRUE(standby);
    TEST_ASSERT_TRUE(ParseStandby(" off\n", standby));
    TEST_ASSERT_FALSE(standby);
    TEST_ASSERT_TRUE(ParseStandby("On", standby));
    TEST_ASSERT_TRUE(standby);
}

void test_ParseStandby_RejectsEverythingElse(void)
{
    const char *invalid[] = {"", "1", "0", "true", "ONN", "OF", "standby"};
    for (const char *payload : invalid)
    {
        bool standby = true;
        TEST_ASSERT_FALSE_MESSAGE(ParseStandby(payload, standby), payload);
        TEST_ASSERT_TRUE_MESSAGE(standby, payload); // untouched on failure
    }
}

// --- PendingCommand ---

void test_PendingCommand_EmptyByDefault(void)
{
    PendingCommand<float> command;
    float value = 0.0f;
    TEST_ASSERT_FALSE(command.Take(value));
}

void test_PendingCommand_TakeReturnsValueOnce(void)
{
    PendingCommand<float> command;
    command.Set(12.0f);

    float value = 0.0f;
    TEST_ASSERT_TRUE(command.Take(value));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.0f, value);
    TEST_ASSERT_FALSE(command.Take(value));
}

void test_PendingCommand_LatestValueWins(void)
{
    PendingCommand<bool> command;
    command.Set(true);
    command.Set(false);

    bool value = true;
    TEST_ASSERT_TRUE(command.Take(value));
    TEST_ASSERT_FALSE(value);
    TEST_ASSERT_FALSE(command.Take(value));
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    UNITY_BEGIN();

    RUN_TEST(test_ParseChargingCurrent_AcceptsPlainNumbers);
    RUN_TEST(test_ParseChargingCurrent_TrimsWhitespace);
    RUN_TEST(test_ParseChargingCurrent_RejectsGarbage);
    RUN_TEST(test_ParseChargingCurrent_RejectsTooLongPayload);
    RUN_TEST(test_ParseChargingCurrent_UsesLengthNotTerminator);
    RUN_TEST(test_ParseChargingCurrent_NullPayload_IsRejected);
    RUN_TEST(test_ParseStandby_AcceptsOnOffCaseInsensitive);
    RUN_TEST(test_ParseStandby_RejectsEverythingElse);
    RUN_TEST(test_PendingCommand_EmptyByDefault);
    RUN_TEST(test_PendingCommand_TakeReturnsValueOnce);
    RUN_TEST(test_PendingCommand_LatestValueWins);

    return UNITY_END();
}
