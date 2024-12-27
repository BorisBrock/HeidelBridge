#pragma once

class ModbusRTU
{
private:
    // Private constructor to prevent instantiation
    ModbusRTU() {};

public:
    // Returns the singleton instance of ModbusRTU
    static ModbusRTU *Instance();

    // Initializes the ModbusRTU instance
    void Init();

    /**
     * Reads multiple registers starting from the specified address.
     * @param startAddress The starting address of the registers to read.
     * @param numValues The number of registers to read.
     * @param fc The function code to use for reading.
     * @param values Pointer to an array where the read values will be stored.
     * @return True if the read operation was successful, false otherwise.
     */
    bool ReadRegisters(uint16_t startAddress, uint8_t numValues, uint8_t fc, uint16_t *values);

    /**
     * Writes a single 16 bit holding register at the specified address.
     * @param address Address of the register to write.
     * @param value 16 bit value to write.
     * @return True if the write operation was successful, false otherwise.
     */
    bool WriteHoldRegister16(uint16_t address, uint16_t value);
};