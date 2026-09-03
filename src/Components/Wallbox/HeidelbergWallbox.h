#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "../../Configuration/Constants.h"
#include "IWallbox.h"
#include "EnergyCounter.h"
#include "EnergyCounterStorage.h"

class HeidelbergWallbox : public IWallbox
{
private:
    HeidelbergWallbox();
    float ClampToWallboxRange(float currentLimitA);
    bool WriteCurrentLimitRegister(float currentLimitA);
    static EnergyCounter::Config EnergyCounterConfig();

    // Guards the energy counter, which is read from the MQTT loop and the Modbus TCP task and set from the web server task
    struct EnergyLock
    {
        explicit EnergyLock(SemaphoreHandle_t mutex) : mMutex(mutex) { xSemaphoreTake(mMutex, portMAX_DELAY); }
        ~EnergyLock() { xSemaphoreGive(mMutex); }
        SemaphoreHandle_t mMutex;
    };

public:
    static HeidelbergWallbox *Instance();

#pragma region IWallbox
    virtual void Init() override;
    virtual void Update() override;
    virtual VehicleState GetState() override;
    virtual bool SetChargingCurrentLimit(float currentLimitA) override;
    virtual bool SetChargingEnabled(bool chargingEnabled) override;
    virtual bool SetStandbyEnabled(bool standbyEnabled) override;
    virtual float GetChargingCurrentLimit() override;
    virtual float GetEnergyMeterValue() override;
    virtual bool HasEnergyMeterValue() override;
    virtual bool GetEnergyMeterDiagnostics(int64_t &offsetWh, uint32_t &rawWh) override;
    virtual bool SetEnergyMeterValue(uint32_t energyWh) override;
    virtual float GetFailsafeCurrent() override;
    virtual float GetChargingPower() override;
    virtual float GetTemperature() override;
    virtual bool GetChargingCurrents(float &c1A, float &c2A, float &c3A) override;
    virtual bool GetChargingVoltages(float &v1V, float &v2V, float &v3V) override;
    virtual bool IsChargingEnabled() override;
    virtual bool GetStandbyEnabled() override;

#pragma endregion IWallbox

private:
    VehicleState mState{VehicleState::Disconnected};
    float mRequestedChargingCurrentLimitA{Constants::HeidelbergWallbox::InitialChargingCurrentLimitA}; // intent
    float mObservedChargingCurrentLimitA{0.0f};                                                        // last read back
    float mFailsafeCurrentA{0.0f};
    float mLastPowerMeterValueW{0.0f};
    EnergyCounterStorage mEnergyCounterStorage;
    EnergyCounter mEnergyCounter;
    SemaphoreHandle_t mEnergyMutex{nullptr};
    bool mChargingEnabled{true}; // seeded from the wallbox in Init()
    bool mStandbyEnabled{true}; // default: standby enabled
};