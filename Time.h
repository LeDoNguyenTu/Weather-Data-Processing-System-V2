#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <stdexcept>
#include <iosfwd>

/**
 * @class Time
 * @file Time.h
 * @brief A simple class that stores a time of day in twenty-four hour format.
 * @details Manages time with support for validation and comparison.
 *          Values are stored internally as hours (0–23), minutes (0–59), and
 *          seconds (0–59).
 *
 * Note: String formatting and parsing are handled by the TimeFormatter class
 * to maintain separation of concerns (MVC pattern).
 */
class Time
{
public:
    /**
     * @brief Default constructor initializing to midnight.
     * @details Creates a Time object set to 00:00:00 (start of day).
     */
    Time();

    /**
     * @brief Parameterized constructor to create a time from given values.
     * @param [in] h Hour value (0–23).
     * @param [in] m Minute value (0–59).
     * @param [in] s Second value (0–59), defaults to 0 if omitted.
     * @throws std::out_of_range if hour, minute, or second values are out of valid range.
     */
    Time(int h, int m, int s = 0);

    /**
     * @brief Set the time with hour, minute, and second values.
     * @param [in] h Hour value (0–23).
     * @param [in] m Minute value (0–59).
     * @param [in] s Second value (0–59), defaults to 0 if omitted.
     * @throws std::out_of_range if hour, minute, or second values are out of valid range.
     */
    void set(int h, int m, int s = 0);

    /**
     * @brief Get the hour component.
     * @return The hour value (0–23).
     */
    int hour() const;

    /**
     * @brief Get the minute component.
     * @return The minute value (0–59).
     */
    int minute() const;

    /**
     * @brief Get the second component.
     * @return The second value (0–59).
     */
    int second() const;

    /**
     * @brief Equality comparison operator.
     * @param [in] o The Time object to compare with.
     * @return True if both times have identical hour, minute, and second values, false otherwise.
     */
    bool operator==(const Time& o) const;

    /**
     * @brief Inequality comparison operator.
     * @param [in] o The Time object to compare with.
     * @return True if the times differ in any component, false otherwise.
     */
    bool operator!=(const Time& o) const
    {
        return !(*this == o);
    }

private:
    int h_;  ///< Hour component (0–23)
    int m_;  ///< Minute component (0–59)
    int s_;  ///< Second component (0–59)
};

/**
 * @brief Stream output operator to write a time.
 * @param [in,out] os Output stream to write to.
 * @param [in]     t  Time object to write.
 * @return The same output stream (allows chaining).
 * @details Outputs the time in fixed-width format "HH:MM:SS" with zero-padded
 *          components. Delegates formatting to TimeFormatter class.
 */
std::ostream& operator<<(std::ostream& os, const Time& t);

#endif // TIME_H_INCLUDED
