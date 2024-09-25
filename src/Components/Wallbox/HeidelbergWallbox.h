#pragma once

#include "../../Configuration/Constants.h"
#include "IWallbox.h"

class HeidelbergWallbox : public IWallbox
{
private:
    HeidelbergWallbox(){};

public:
    static HeidelbergWallbox *Instance();

#pragma region IWallbox
    virtual void Init() override;
    virtual VehicleState GetState() override;
    virtual bool SetChargingCurrentLimit(float currentLimitA) override;
    virtual float GetChargingCurrentLimit() override;
    virtual float GetEnergyMeterValue() override;
    virtual float GetFailsafeCurrent() override;
    virtual float GetChargingPower() override;
    virtual float GetTemperature() override;
    virtual bool GetChargingCurrents(float &c1A, float &c2A, float &c3A) override;
    virtual bool GetChargingVoltages(float &v1V, float &v2V, float &v3V) override;
#pragma endregion IWallbox

private:
    VehicleState mState{VehicleState::Disconnected};
    float mChargingCurrentLimitA{Constants::HeidelbergWallbox::InitialChargingCurrentLimitA};
    float mFailsafeCurrentA{0.0f};
    float mLastPowerMeterValueW{0.0f};
    float mLastEnergyMeterValueWh{0.0f};
};