#include <Arduino.h>
#include "PrefixedString.h"

PrefixedString::PrefixedString(uint16_t capacity)
    : mCapacity(capacity)
{
    mBuffer = new char[mCapacity];
    memset(mBuffer, 0, mCapacity);
}

// Destructor
PrefixedString::~PrefixedString()
{
    delete[] mBuffer;
}

// Sets the prefix
void PrefixedString::SetPrefix(const char *prefix)
{
    mPrefixLength = strlen(prefix);
    strncpy(mBuffer, prefix, mCapacity);
}

// Sets the string to the provided value
const char* PrefixedString::SetString(const char *value)
{
    memset(mBuffer + mPrefixLength, 0, mCapacity - mPrefixLength);
    strncpy(mBuffer + mPrefixLength, value, mCapacity - mPrefixLength);
    return mBuffer;
}

// Gets the full string including prefix
const char *PrefixedString::GetString() const
{
    return mBuffer;
}
