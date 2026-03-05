#ifndef DATETIME_UTILS_H
#define DATETIME_UTILS_H

#include "WeatherReading.h"

/**
 * @class DateTimeUtils
 * @brief Utility functions for date/time operations and filtering.
 * @details Provides static helper methods for date/time comparisons,
 *          month name lookup, and timestamp key generation.
 *
 * Responsibilities:
 * - Test if reading matches month/year filter
 * - Convert month number to English name
 * - Generate unique timestamp keys for deduplication
 */
class DateTimeUtils
{
public:
    /**
     * @brief Test whether a reading belongs to a specific month and year.
     * @param r Candidate weather reading
     * @param year Target year (e.g., 2015)
     * @param month Target month (1=January, 12=December)
     * @return true if reading's date matches month/year, false otherwise
     *
     * @details Used to filter readings during tree traversal.
     * Compares reading's date components against target month/year.
     */
    static bool isMonthYear(const WeatherReading& r, int year, int month);

    /**
     * @brief Get English name of a month for display/reporting.
     * @param m Month number (1=January, 12=December)
     * @return Pointer to month name string (e.g., "September"), or "?" if invalid
     *
     * @details Returns static string - do not free or modify.
     * Valid input range: 1-12. Returns "?" for out-of-range values.
     */
    static const char* monthName(int m);

    /**
     * @brief Generate unique timestamp key for deduplication.
     * @param d Date of reading
     * @param t Time of reading
     * @return 64-bit key uniquely identifying this date/time combination
     *
     * @details Combines date and time into single unsigned long long value.
     * Used as key in Map for O(1) duplicate detection during CSV loading.
     * Formula encodes year, month, day, hour, minute, second into 64 bits.
     */
    static unsigned long long timestampKey(const Date& d, const Time& t);
};

#endif // DATETIME_UTILS_H
