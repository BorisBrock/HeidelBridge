#pragma once

namespace StringUtils
{
    // Inserts a string into another string, replacing a wildcard character
    void InsertString(const char *inputString, char *outputString, size_t outputLength, char wildcardCharacter, const char *replacementString);
}