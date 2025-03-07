#include <Arduino.h>
#include "StringUtils.h"

namespace StringUtils
{

    // Inserts a string into another string, replacing a wildcard character
    void InsertString(const char *inputString, char *outputString, size_t outputLength, char wildcardCharacter, const char *replacementString)
    {
        size_t inputLen = strlen(inputString);
        size_t replacementLen = strlen(replacementString);
        size_t outputIndex = 0;

        for (size_t i = 0; i < inputLen && outputIndex < outputLength - 1; i++)
        {
            if (inputString[i] == wildcardCharacter)
            {
                // Check if there is enough space to insert the replacement string
                if (outputIndex + replacementLen >= outputLength - 1)
                {
                    break; // Stop if we exceed the buffer limit
                }
                strcpy(&outputString[outputIndex], replacementString);
                outputIndex += replacementLen;
            }
            else
            {
                outputString[outputIndex++] = inputString[i];
            }
        }

        outputString[outputIndex] = '\0'; // Null-terminate the result
    }
}
