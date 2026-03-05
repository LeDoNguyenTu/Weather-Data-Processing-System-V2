#include "WeatherReading.h"

// Default constructor - initializes all members with their default values
WeatherReading::WeatherReading()
    : d(), t(), wind_ms(0.0f), temp_c(0.0f), solar_wm2(0.0f) {}

// Parameterized constructor - copies all parameters into member variables
WeatherReading::WeatherReading(const Date& d_, const Time& t_, float s_ms, float t_C, float sr_Wm2)
    : d(d_), t(t_), wind_ms(s_ms), temp_c(t_C), solar_wm2(sr_Wm2) {}

// Less-than operator for chronological ordering
// Compares two weather readings to determine which occurred first in time
// Used by AVL tree for maintaining sorted order
bool WeatherReading::operator<(const WeatherReading& other) const
{
    // Step 1: Compare dates using the Date compareTo method
    int cmp = d.compareTo(other.d);
    // If dates are different, use the date comparison result to determine order
    if (cmp != 0) return cmp < 0;

    // Step 2: Dates are equal, so compare hours
    // If hours differ, return true if this hour is earlier
    if (t.hour() != other.t.hour()) return t.hour() < other.t.hour();

    // Step 3: Hours are equal, so compare minutes
    // If minutes differ, return true if this minute is earlier
    if (t.minute() != other.t.minute()) return t.minute() < other.t.minute();

    // Step 4: Hours and minutes are equal, so compare seconds
    // Return true if this second is earlier than the other second
    return t.second() < other.t.second();
}

// Equality operator for chronological comparison
// Checks if two weather readings occurred at exactly the same date and time
bool WeatherReading::operator==(const WeatherReading& other) const
{
    // Step 1: Compare dates using the Date compareTo method
    // Step 2: Compare hours, minutes, and seconds separately
    // Return true only if all components are identical
    return d.compareTo(other.d) == 0 &&
           t.hour() == other.t.hour() &&
           t.minute() == other.t.minute() &&
           t.second() == other.t.second();
}
