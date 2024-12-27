#pragma once

enum VehicleState
{
    Disconnected = 1,
    Connected = 2,
    Charging = 4
};

class IWallbox
{
public:
    // Destructor
    virtual ~IWallbox() = default;

    // Initialization function
    virtual void Init() = 0;

    // Read functions

    // Returns the current state of the vehicle
    virtual VehicleState GetState() = 0;

    // Returns the current charging current limit in Amperes
    virtual float GetChargingCurrentLimit() = 0;

    // Returns the current energy meter value in kilowatt hours
    virtual float GetEnergyMeterValue() = 0;

    // Returns the current failsafe current in Amperes
    virtual float GetFailsafeCurrent() = 0;

    // Returns the current charging power in kilowatts
    virtual float GetChargingPower() = 0;

    // Returns the current temperature in degrees Celsius
    virtual float GetTemperature() = 0;

    // Returns the current charging currents in Amperes
    virtual bool GetChargingCurrents(float &c1A, float &c2A, float &c3A) = 0;

    // Returns the current charging voltages in Volts
    virtual bool GetChargingVoltages(float &v1V, float &v2V, float &v3V) = 0;

    // Write functions

    // Sets the charging current limit in Amperes
    virtual bool SetChargingCurrentLimit(float currentLimitA) = 0;
};