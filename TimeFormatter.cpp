#include "TimeFormatter.h"
#include <cstdio>
#include <sstream>
#include <iomanip>

// ======================= FORMAT: TIME TO STRING =======================

// Convert Time object to "HH:MM:SS" string format
// Returns string with zero-padded components
std::string TimeFormatter::format(const Time& time)
{
    // Step 1: Create character buffer for formatted time string
    // Buffer size: 8 characters (HH:MM:SS) + 1 null terminator = 9 bytes
    char buf[9];

    // Step 2: Format time into buffer using snprintf
    // %02d - integer with minimum 2 digits, zero-padded if needed
    // : - literal colon separator between components
    // Example: hour=7, minute=5, second=3 -> "07:05:03"
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
                  time.hour(), time.minute(), time.second());

    // Step 3: Create and return std::string from C-string buffer
    // Automatic conversion from char array to std::string
    return std::string(buf);
}

// ======================= PARSE: STRING TO TIME =======================

// Parse time string and return Time object
// Supports formats: "HH:MM" (seconds default to 0) or "HH:MM:SS"
Time TimeFormatter::fromString(const std::string& str)
{
    // Step 1: Initialize hour, minute, second to 0
    int h = 0, m = 0, s = 0;

    // ===== PARSE: Count colons to determine format =====
    // Step 2: Count number of colons in string to detect format
    // 1 colon = "HH:MM" format
    // 2 colons = "HH:MM:SS" format
    int colons = 0;
    for (std::size_t i = 0; i < str.size(); ++i)
    {
        // Step 2a: Check each character for colon
        if (str[i] == ':') ++colons;
    }

    // Step 3: Process based on colon count
    if (colons == 1)
    {
        // ===== FORMAT: HH:MM (seconds default to 0) =====
        // Step 3a: Format is HH:MM - parse two integers with colon separator
        // Leading spaces accepted by %d in sscanf
        // Example: " 7:05" or "07:05" both valid
        int n = std::sscanf(str.c_str(), " %d:%d", &h, &m);

        // Step 3b: Check if exactly 2 values were parsed (hour and minute)
        if (n != 2)
        {
            // Step 3c: If parsing failed, throw exception with original string
            throw std::invalid_argument("TimeFormatter::fromString bad format: " + str);
        }

        // Step 3d: Set seconds to 0 (default for HH:MM format)
        s = 0;
    }
    else if (colons == 2)
    {
        // ===== FORMAT: HH:MM:SS (full format) =====
        // Step 4a: Format is HH:MM:SS - parse three integers
        // Example: "7:05:03" or "07:05:03" both valid
        int n = std::sscanf(str.c_str(), " %d:%d:%d", &h, &m, &s);

        // Step 4b: Check if exactly 3 values were parsed (hour, minute, second)
        if (n != 3)
        {
            // Step 4c: If parsing failed, throw exception with original string
            throw std::invalid_argument("TimeFormatter::fromString bad format: " + str);
        }
    }
    else
    {
        // ===== FORMAT: INVALID (wrong number of colons) =====
        // Step 5: If colon count not 1 or 2, format is invalid
        throw std::invalid_argument("TimeFormatter::fromString bad format: " + str);
    }

    // Step 6: Create and return Time object with parsed values
    // Note: Time constructor calls set() which will validate ranges
    // If hour/minute/second out of range, Time constructor throws exception
    return Time(h, m, s);
}

// ======================= STREAM INPUT PARSING =======================

// Parse time from input stream
// Reads one whitespace-delimited token and tries to parse as time
// Returns true if successful, false if parsing fails
bool TimeFormatter::tryParseFromStream(std::istream& in, Time& outTime)
{
    // Step 1: Skip leading whitespace in input stream
    // std::ws manipulator consumes all whitespace characters
    in >> std::ws;

    // Step 2: Declare string to hold parsed token
    std::string token;

    // Step 3: Attempt to read next whitespace-delimited token from stream
    // operator>> stops at first whitespace
    if (!(in >> token)) return false;

    // Step 4: Try parsing token as time using fromString
    try
    {
        // Step 4a: Attempt to parse token as Time
        outTime = fromString(token);

        // Step 4b: Parsing succeeded - return true
        return true;
    }
    catch (...)
    {
        // Step 4c: Parsing failed (invalid format or out of range)
        // Return false without modifying outTime
        return false;
    }
}
