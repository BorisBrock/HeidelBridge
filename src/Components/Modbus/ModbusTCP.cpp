#include <Arduino.h>
#include <WiFi.h>
#include "../Logger/Logger.h"
#include "ModbusServerWiFi.h"
#include "../../Configuration/Constants.h"
#include "../Wallbox/IWallbox.h"
#include "../../Utils/DeviceInfo.h"
#include "DaheimladenRegisterMap.h"
#include "ModbusTCP.h"

namespace ModbusTCP
{
    ModbusServerWiFi gModbusServer;
    IWallbox *gWallbox = nullptr;

    // Reads a single input register
    ModbusMessage ReadInputRegister(ModbusMessage msg)
    {
        // For debugging
        Logger::Trace("Modbus TCP request received: READ_INPUT_REGISTER:");
        Logger::Trace(" > Size: %d", msg.size());
        Logger::Trace(" > FC: %d", msg.getFunctionCode());
        Logger::Trace(" > Data:");
        for (uint8_t i = 0; i < msg.size(); ++i)
        {
            Logger::Trace("   > %d", msg[i]);
        }
        return ModbusMessage{};
    }

    // Reads one or more consecutive holding registers
    ModbusMessage ReadHoldRegister(ModbusMessage request)
    {
        uint8_t fc = request.getFunctionCode();
        uint16_t startAddress = 0;
        uint16_t numWords = 0;
        request.get(2, startAddress);
        request.get(4, numWords);
        Logger::Trace("ModbusTCP request received: READ_HOLD_REGISTER %d words at reg. %d", numWords, startAddress);

        ModbusMessage response;
        if (numWords == 0 || numWords > DaheimladenRegisterMap::MaxReadWords)
        {
            Logger::Error(" -> Responding with error ILLEGAL_DATA_VALUE");
            response.setError(request.getServerID(), fc, Modbus::Error::ILLEGAL_DATA_VALUE);
            return response;
        }

        uint16_t words[DaheimladenRegisterMap::MaxReadWords];
        const String serial = DeviceInfo::GetDeviceSerialNumber();
        if (!DaheimladenRegisterMap::Read(*gWallbox, serial.c_str(), startAddress, numWords, words))
        {
            Logger::Error(" -> Responding with error ILLEGAL_DATA_ADDRESS");
            response.setError(request.getServerID(), fc, Modbus::Error::ILLEGAL_DATA_ADDRESS);
            return response;
        }

        response.add(request.getServerID(), fc, static_cast<uint8_t>(numWords * 2));
        for (uint16_t i = 0; i < numWords; ++i)
            response.add(words[i]);
        return response;
    }

    // Writes a single holding register
    ModbusMessage WriteHoldRegister(ModbusMessage msg)
    {
        // For debugging only
        Logger::Trace("Modbus TCP request received: WRITE_HOLD_REGISTER");
        Logger::Trace(" > Size: %d", msg.size());
        Logger::Trace(" > FC: %d", msg.getFunctionCode());
        Logger::Trace(" > Data:");
        for (uint8_t i = 0; i < msg.size(); ++i)
        {
            Logger::Trace("   > %d", msg[i]);
        }
        return msg; // Echo back request
    }

    // Writes multiple holding registers
    ModbusMessage WriteMultipleRegisters(ModbusMessage request)
    {
        uint8_t fc = request.getFunctionCode();
        uint16_t startAddress = 0;
        uint16_t numWords = 0;
        request.get(2, startAddress);
        request.get(4, numWords);
        Logger::Trace("ModbusTCP request received: WRITE_MULT_REGISTERS %d words to reg. %d", numWords, startAddress);

        // Respond properly
        switch (startAddress)
        {
        case (Constants::DaheimladenRegisters::LimitChargingCurrent):
        {
            uint16_t currentLimit = 0;
            request.get(7, currentLimit);
            Logger::Trace(" -> Writing charging current limit: %d", currentLimit);
            gWallbox->SetChargingCurrentLimit(static_cast<float>(currentLimit * 0.1f));
            break;
        }
        default:
        {
            Logger::Error(" -> Responding with error ILLEGAL_DATA_ADDRESS");
            ModbusMessage response;
            response.setError(request.getServerID(), fc, Modbus::Error::ILLEGAL_DATA_ADDRESS);
            return response;
        }
        }

        // Respond with register address and register count
        ModbusMessage response;
        response.add(request.getServerID(), fc, startAddress);
        response.add(numWords);
        return response;
    }

    // Initializes the ModbusTCP server
    void Init(IWallbox *wallbox)
    {
        Logger::Info("Initializing Modbus TCP server");

        gWallbox = wallbox;

        gModbusServer.start(
            Constants::DaheimladenWallbox::TCPPort,
            Constants::DaheimladenWallbox::MaxClients,
            Constants::DaheimladenWallbox::TimeoutMs);

        gModbusServer.registerWorker(Constants::DaheimladenWallbox::ServerId, READ_INPUT_REGISTER, ReadInputRegister);
        gModbusServer.registerWorker(Constants::DaheimladenWallbox::ServerId, READ_HOLD_REGISTER, ReadHoldRegister);
        gModbusServer.registerWorker(Constants::DaheimladenWallbox::ServerId, WRITE_HOLD_REGISTER, WriteHoldRegister);
        gModbusServer.registerWorker(Constants::DaheimladenWallbox::ServerId, WRITE_MULT_REGISTERS, WriteMultipleRegisters);
    }
};