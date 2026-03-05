#ifndef DATE_H_INCLUDED
#define DATE_H_INCLUDED
#include <istream>
#include <iostream>

/**
 * @class Date
 * @brief A simple calendar date with day, month, and year.
 * @details Manages calendar dates with support for validation, leap year detection,
 *          date arithmetic, and chronological comparison. Handles all edge cases
 *          including varying month lengths and leap years.
 */
class Date
{
public:
    /**
     * @brief Default constructor initializing to 1/1/1000.
     */
    Date();

    /**
     * @brief Parameterized constructor to create a date from given numbers.
     * @param [in] day   Day of month (1–31 depending on month).
     * @param [in] month Month number (1–12).
     * @param [in] year  Year number (e.g., 2025).
     */
    Date(int day, int month, int year);

    /**
     * @brief Set the day number.
     * @param [in] day The day value to set (1–31 depending on month).
     */
    void SetDay(int day);

    /**
     * @brief Get the day number.
     * @return The day value (1–31).
     */
    int GetDay() const;

    /**
     * @brief Set the month number.
     * @param [in] month The month value to set (1–12).
     */
    void SetMonth(int month);

    /**
     * @brief Get the month number.
     * @return The month value (1–12).
     */
    int GetMonth() const;

    /**
     * @brief Set the year number.
     * @param [in] year The year value to set (e.g., 2025).
     */
    void SetYear(int year);

    /**
     * @brief Get the year number.
     * @return The year value.
     */
    int GetYear() const;

    /**
     * @brief Check if the stored year is a leap year.
     * @return True if this date's year is a leap year, false otherwise.
     * @details A leap year is divisible by 400, or divisible by 4 but not by 100.
     */
    bool isLeapYear() const;

    /**
     * @brief Validate if the stored day/month/year make a real date.
     * @return True if the date is valid, false otherwise.
     * @details Validates month range (1–12), day range for the given month
     *          (accounting for days in each month), and leap year rules.
     */
    bool isValid() const;

    /**
     * @brief Calculate a new date moved by a specified number of days.
     * @param [in] n Number of days to move (negative values move backwards).
     * @return A new Date object with the adjusted date.
     * @details This date object is not changed; a new one is returned.
     */
    Date addDays(int n) const;

    /**
     * @brief Compare two dates chronologically.
     * @param [in] other The date to compare with.
     * @return -1 if this date is earlier, 0 if equal, 1 if this date is later.
     */
    int compareTo(const Date& other) const;

    /**
     * @brief Equality comparison operator for dates.
     * @param [in] other The date to compare with.
     * @return True if both dates have identical day, month, and year, false otherwise.
     */
    bool operator==(const Date& other) const;

    /**
     * @brief Less-than comparison operator for chronological ordering.
     * @param [in] other The date to compare with.
     * @return True if this date is earlier than the other date, false otherwise.
     */
    bool operator<(const Date& other) const;

private:
    int m_day;      ///< Day of the month (1–31)
    int m_month;    ///< Month number (1–12)
    int m_year;     ///< Year number
};

/**
 * @brief Stream output operator to write a date.
 * @param [in,out] os   Output stream to write to.
 * @param [in]     date Date object to write.
 * @return The same output stream (allows chaining).
 * @details Outputs the date in format "d/m/yyyy" with no zero padding.
 *          Example: 3/4/2025 instead of 03/04/2025.
 */
std::ostream &operator<<(std::ostream &os, const Date &date);

/**
 * @brief Stream input operator to read a date.
 * @param [in,out] input Input stream to read from.
 * @param [out]    date  Date object to populate with the parsed date.
 * @return The same input stream (allows chaining).
 * @details Parses the next whitespace-delimited token from the input stream.
 *          Supported formats (one token only):
 *          - 18-Sep-2025 (text month)
 *          - 18/9/2025   (numeric month, no padding)
 *          - 18-09-2025  (numeric month, zero-padded)
 *          - 20250918    (YYYYMMDD format)
 *
 *          If parsing fails, the stream's fail bit is set and the date
 *          object remains unchanged.
 */
std::istream &operator>>(std::istream &input, Date &date);

#endif // DATE_H_INCLUDED
