#include <cstdint>
#include <cstring>
#include "../../Configuration/Constants.h"
#include "DaheimladenRegisterMap.h"

namespace DaheimladenRegisterMap
{
    namespace
    {
        constexpr uint16_t MaxValueWords = 16;

        using Fill = void (*)(IWallbox &wallbox, const char *stationId, uint16_t *words);

        // A value in the register map. Like on a Daheimladen wallbox, every Uint16 value is
        // followed by an unused register, so consecutive values form one contiguous block.
        struct Value
        {
            uint16_t address;
            uint16_t numWords;
            Fill fill;
        };

        uint16_t ToCurrent(float currentA)
        {
            return static_cast<uint16_t>(currentA * Constants::DaheimladenWallbox::CurrentFactor);
        }

        uint16_t ToVoltage(float voltageV)
        {
            return static_cast<uint16_t>(voltageV * Constants::DaheimladenWallbox::VoltageFactor);
        }

        void PutUint32(uint16_t *words, uint32_t value)
        {
            words[0] = static_cast<uint16_t>(value >> 16);
            words[1] = static_cast<uint16_t>(value & 0xFFFF);
        }

        const Value Values[] = {
            {Constants::DaheimladenRegisters::Status, 2,
             [](IWallbox &wallbox, const char *, uint16_t *words)
             { words[0] = static_cast<uint16_t>(wallbox.GetState()); }},
            {Constants::DaheimladenRegisters::ChargeCurrents, 6,
             [](IWallbox &wallbox, const char *, uint16_t *words)
             {
                 float c1 = 0.0f, c2 = 0.0f, c3 = 0.0f;
                 wallbox.GetChargingCurrents(c1, c2, c3);
                 words[0] = ToCurrent(c1);
                 words[2] = ToCurrent(c2);
                 words[4] = ToCurrent(c3);
             }},
            {Constants::DaheimladenRegisters::TotalChargingPower, 2,
             [](IWallbox &wallbox, const char *, uint16_t *words)
             { PutUint32(words, static_cast<uint32_t>(wallbox.GetChargingPower())); }},
            {Constants::DaheimladenRegisters::EnergyMeter, 2,
             [](IWallbox &wallbox, const char *, uint16_t *words)
             { PutUint32(words, static_cast<uint32_t>(wallbox.GetEnergyMeterValue() * Constants::DaheimladenWallbox::EnergyFactor)); }},
            {Constants::DaheimladenRegisters::EvseMaxCurrent, 2,
             [](IWallbox &, const char *, uint16_t *words)
             { words[0] = ToCurrent(Constants::HeidelbergWallbox::MaxChargingCurrentA); }},
            {Constants::DaheimladenRegisters::EvseMinCurrent, 2,
             [](IWallbox &, const char *, uint16_t *words)
             { words[0] = ToCurrent(Constants::HeidelbergWallbox::MinChargingCurrentA); }},
            {Constants::DaheimladenRegisters::CableMaxCurrent, 2,
             [](IWallbox &, const char *, uint16_t *words)
             { words[0] = ToCurrent(Constants::HeidelbergWallbox::MaxChargingCurrentA); }},
            {Constants::DaheimladenRegisters::RfidStationId, 16,
             [](IWallbox &, const char *stationId, uint16_t *words)
             {
                 // Two characters per register, first one in the high byte, zero padded
                 const size_t length = strnlen(stationId, 2 * MaxValueWords);
                 for (size_t i = 0; i < length; ++i)
                     words[i / 2] |= static_cast<uint16_t>(static_cast<uint8_t>(stationId[i])) << (i % 2 == 0 ? 8 : 0);
             }},
            // The currently scanned RFID card, not supported: reads as empty
            {Constants::DaheimladenRegisters::RfidCardId, 16,
             [](IWallbox &, const char *, uint16_t *) {}},
            {Constants::DaheimladenRegisters::MaxChargingCurrentAfterConnectionLoss, 2,
             [](IWallbox &wallbox, const char *, uint16_t *words)
             { words[0] = ToCurrent(wallbox.GetFailsafeCurrent()); }},
            {Constants::DaheimladenRegisters::ConnectionTimeoutTime, 2,
             [](IWallbox &, const char *, uint16_t *words)
             { words[0] = 60; }},
            {Constants::DaheimladenRegisters::LimitChargingCurrent, 2,
             [](IWallbox &wallbox, const char *, uint16_t *words)
             { words[0] = ToCurrent(wallbox.GetChargingCurrentLimit()); }},
            {Constants::DaheimladenRegisters::ChargeVoltages, 6,
             [](IWallbox &wallbox, const char *, uint16_t *words)
             {
                 float v1 = 0.0f, v2 = 0.0f, v3 = 0.0f;
                 wallbox.GetChargingVoltages(v1, v2, v3);
                 words[0] = ToVoltage(v1);
                 words[2] = ToVoltage(v2);
                 words[4] = ToVoltage(v3);
             }},
        };

        bool IsMapped(uint32_t address)
        {
            for (const Value &value : Values)
            {
                if (address >= value.address && address < static_cast<uint32_t>(value.address) + value.numWords)
                    return true;
            }
            return false;
        }
    }

    bool Read(IWallbox &wallbox, const char *stationId, uint16_t start, uint16_t count, uint16_t *out)
    {
        const uint32_t end = static_cast<uint32_t>(start) + count;
        for (uint32_t address = start; address < end; ++address)
        {
            if (!IsMapped(address))
                return false;
        }

        // Each value is read from the wallbox once, even if the request covers it only partly
        for (const Value &value : Values)
        {
            const uint32_t valueEnd = static_cast<uint32_t>(value.address) + value.numWords;
            if (valueEnd <= start || value.address >= end)
                continue;

            uint16_t words[MaxValueWords] = {};
            value.fill(wallbox, stationId, words);
            const uint32_t from = value.address > start ? value.address : start;
            const uint32_t to = valueEnd < end ? valueEnd : end;
            for (uint32_t address = from; address < to; ++address)
                out[address - start] = words[address - value.address];
        }
        return true;
    }
};
