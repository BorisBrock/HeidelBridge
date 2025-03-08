#pragma once

class PrefixedString
{
public:
    // Constructor;
    PrefixedString(uint16_t capacity);

    // Destructor
    ~PrefixedString();

    // Sets the prefix
    void SetPrefix(const char *prefix);

    // Sets the string to the provided value
    const char* SetString(const char *value);

    // Gets the full string including prefix
    const char* GetString() const;

private:
    uint16_t mCapacity{};
    uint16_t mPrefixLength{};
    char *mBuffer{};
};
