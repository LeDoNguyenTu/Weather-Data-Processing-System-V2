#ifndef TIMEFORMATTER_H_INCLUDED
#define TIMEFORMATTER_H_INCLUDED

#include <string>
#include <iosfwd>
#include <stdexcept>
#include "Time.h"

/**
 * @file TimeFormatter.h
 * @brief Formatting and parsing utilities for the Time type.
 *
 * @details
 * This component converts Time values to text and converts text back to Time values.
 * It supports two input formats:
 * - "HH:MM" which is hour colon minute (seconds default to 0)
 * - "HH:MM:SS" which is hour colon minute colon second
 *
 * Output format is always "HH:MM:SS" with zero-padded components.
 *
 * Design follows MVC pattern: Time class holds data only, TimeFormatter handles
 * all string conversions (View responsibility).
 */

/**
 * @class TimeFormatter
 * @brief Convert Time values to text and parse text into Time values.
 *
 * @details
 * Use the format function to produce text. Use the fromString function to read text.
 * All static utility class - no instances created.
 */
class TimeFormatter
{
public:
    /**
     * @brief Format a Time object to string in "HH:MM:SS" format.
     * @param time The time to format.
     * @return A string with zero-padded components (e.g., "07:05:03").
     *
     * @details
     * Converts time to fixed-width "HH:MM:SS" format where each component
     * is padded to 2 digits with leading zeros if necessary.
     * Examples: "00:00:00" (midnight), "07:05:03", "23:59:59"
     */
    static std::string format(const Time& time);

    /**
     * @brief Parse a string to create a Time object.
     * @param str The time string in "HH:MM" or "HH:MM:SS" format.
     * @return A new Time object with the parsed values.
     * @throws std::invalid_argument if the string format is invalid or values are out of range.
     *
     * @details Supports two formats:
     * - "HH:MM" - seconds default to 0
     * - "HH:MM:SS" - full time specification
     *
     * Examples of valid inputs: "7:05", "07:05", "7:05:03", "23:59:58"
     */
    static Time fromString(const std::string& str);

    /**
     * @brief Try to parse a Time from an input stream without throwing exceptions.
     * @param in The input stream to read from.
     * @param outTime This value is set to the parsed time when parsing succeeds.
     * @return True when parsing succeeds, false when parsing fails.
     *
     * @details Reads one whitespace-delimited token from the stream and tries to parse it.
     * Does not set stream fail bit on parse failure. Useful for optional time input.
     */
    static bool tryParseFromStream(std::istream& in, Time& outTime);
};

#endif // TIMEFORMATTER_H_INCLUDED
