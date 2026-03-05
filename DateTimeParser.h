#ifndef DATETIME_PARSER_H
#define DATETIME_PARSER_H

#include <string>
#include "Date.h"
#include "Time.h"

/**
 * @class DateTimeParser
 * @brief Parse combined date/time strings from weather data.
 * @details Handles WAST (Western Australian Standard Time) timestamp parsing.
 *          Supports both ISO and Australian date formats.
 *
 * Responsibilities:
 * - Parse WAST timestamp field from CSV
 * - Support multiple date/time formats
 * - Validate parsed dates
 */
class DateTimeParser
{
public:
    /**
     * @brief Parse WAST (date/time) field from CSV.
     * @param field WAST CSV field string
     * @param d Parsed Date object (output)
     * @param t Parsed Time object (output)
     * @return true if parsing succeeded and date is valid, false on error
     *
     * @details Supports two formats:
     * - ISO format: "YYYY-MM-DD HH:MM:SS" (e.g., "2015-09-18 07:05:00")
     * - Australian format: "D/M/YYYY HH:MM:SS" (e.g., "18/9/2015 07:05:00")
     *
     * Validates date using Date::isValid() after parsing.
     */
    static bool parseWAST(const std::string& field, Date& d, Time& t);
};

#endif // DATETIME_PARSER_H
