#include "Time.h"
#include "TimeFormatter.h"
#include <ostream>
#include <iomanip>

// =================== DEFAULT CONSTRUCTOR ===================

// Initialize time to midnight (00:00:00)
Time::Time() : h_(0), m_(0), s_(0) {}

// =================== PARAMETERIZED CONSTRUCTOR ===================

// Initialize time from hour, minute, second with range validation
Time::Time(int h, int m, int s) : h_(0), m_(0), s_(0)
{
    // Step 1: Initialize all members to 0 (safe default)
    // Step 2: Call set() to validate and assign parameters
    // set() will throw exception if values out of range
    set(h, m, s);
}

// =================== SETTER WITH VALIDATION ===================

// Set hour, minute, second with range checking
// Throws std::out_of_range if values outside valid ranges
void Time::set(int h, int m, int s)
{
    // Step 1: Validate hour is in range [0, 23]
    // Step 2: Validate minute is in range [0, 59]
    // Step 3: Validate second is in range [0, 59]
    if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59)
    {
        // Step 4: If any value out of range, throw exception
        throw std::out_of_range("Time::set invalid h:m:s");
    }

    // Step 5: All values valid - assign to member variables
    h_ = h;
    m_ = m;
    s_ = s;
}

// =================== GETTERS ===================

// Return hour component (0-23)
int Time::hour() const
{
    return h_;
}

// Return minute component (0-59)
int Time::minute() const
{
    return m_;
}

// Return second component (0-59)
int Time::second() const
{
    return s_;
}

// =================== COMPARISON OPERATORS ===================

// Equality comparison - check if two times are identical
bool Time::operator==(const Time& o) const
{
    // Step 1: Compare hour component
    // Step 2: Compare minute component
    // Step 3: Compare second component
    // Step 4: Return true only if ALL components match
    return h_ == o.h_ && m_ == o.m_ && s_ == o.s_;
}

// =================== STREAM OUTPUT OPERATOR ===================

// Output stream operator to write time in "HH:MM:SS" format
// Operator overload allows: std::cout << time_object;
std::ostream& operator<<(std::ostream& os, const Time& t)
{
    // Step 1: Delegate formatting to TimeFormatter class (MVC pattern)
    // TimeFormatter handles all string conversion logic
    os << TimeFormatter::format(t);

    // Step 2: Return output stream to allow stream chaining
    return os;
}
