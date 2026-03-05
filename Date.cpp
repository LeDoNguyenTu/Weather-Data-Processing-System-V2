#include "Date.h"
#include "DateFormatter.h"

// ======================= CONSTRUCTORS =======================

// Default constructor - initialize to 1 January 1000
Date::Date() : m_day(1), m_month(1), m_year(1000) {}

// Parameterized constructor - initialize with specific day, month, year
// No validation performed (caller responsibility)
Date::Date(int day, int month, int year)
    : m_day(day), m_month(month), m_year(year) {}

// ======================= SETTERS =======================

// Set day component (1-31)
void Date::SetDay(int day)
{
    // Step 1: Store day value directly (no validation)
    m_day = day;
}

// Set month component (1-12)
void Date::SetMonth(int month)
{
    // Step 1: Store month value directly (no validation)
    m_month = month;
}

// Set year component
void Date::SetYear(int year)
{
    // Step 1: Store year value directly (no validation)
    m_year = year;
}

// ======================= GETTERS =======================

// Get day component
int Date::GetDay() const
{
    // Step 1: Return day value
    return m_day;
}

// Get month component
int Date::GetMonth() const
{
    // Step 1: Return month value
    return m_month;
}

// Get year component
int Date::GetYear() const
{
    // Step 1: Return year value
    return m_year;
}

// ======================= STREAM OUTPUT =======================

// Output stream operator - print date in long month format
// Format: d-MMM-yyyy (e.g., 18-Sep-2025)
std::ostream &operator<<(std::ostream &os, const Date &date)
{
    // Step 1: Use DateFormatter to format date as DMY_MON_SHORT style
    // Step 2: Output formatted string to stream
    // Step 3: Return stream to allow chaining
    return os << DateFormatter::format(date, DateFormatter::Style::DMY_MON_SHORT);
}

// ======================= STREAM INPUT =======================

// Input stream operator - parse date from stream
// Accepts multiple formats via DateFormatter::tryParseFromStream
std::istream &operator>>(std::istream &input, Date &date)
{
    // Step 1: Create temporary Date object to hold parsed result
    Date parsed;

    // Step 2: Attempt to parse date from input stream
    if (DateFormatter::tryParseFromStream(input, parsed))
    {
        // Step 2a: Parsing succeeded - copy parsed components to target date
        date.SetDay(parsed.GetDay());
        date.SetMonth(parsed.GetMonth());
        date.SetYear(parsed.GetYear());
    }
    else
    {
        // Step 2b: Parsing failed - print error message to stderr
        std::cerr << "Error: Invalid date format. Supported: d-MMM-yyyy, d/M/yyyy, d-M-yyyy, yyyyMMdd.\n";

        // Step 2c: Set failbit on input stream to indicate error
        input.setstate(std::ios::failbit);
    }

    // Step 3: Return input stream to allow chaining
    return input;
}

// ======================= EQUALITY OPERATOR =======================

// Equality comparison - check if two dates are identical
bool Date::operator==(const Date& other) const
{
    // Step 1: Compare year, month, and day
    // Step 2: Return true only if all three match
    return (this->m_year == other.m_year) && (this->m_month == other.m_month) && (this->m_day == other.m_day);
}

// ======================= LESS-THAN OPERATOR =======================

// Less-than comparison for chronological ordering
// Used for sorting and container insertion (e.g., AVL tree)
bool Date::operator<(const Date& other) const
{
    // Step 1: Compare years first (most significant)
    if (this->m_year != other.m_year)
        return this->m_year < other.m_year;

    // Step 2: If years equal, compare months
    if (this->m_month != other.m_month)
        return this->m_month < other.m_month;

    // Step 3: If months equal, compare days (least significant)
    return this->m_day < other.m_day;
}

// ======================= ANONYMOUS NAMESPACE (INTERNAL HELPERS) =======================

namespace
{
    // ===== DAYS FROM CIVIL ALGORITHM =====

    // Calculate days since Unix epoch (1970-01-01) for given civil date
    // Uses Gregorian calendar; algorithm by Howard Hinnant (public domain)
    // This converts a calendar date to an absolute day count for date arithmetic
    inline long long daysFromCivil(int y, int m, int d)
    {
        // Step 1: Adjust year if month is Jan/Feb (before March)
        // In this algorithm, year cycle starts in March, so Jan/Feb belong to previous "year"
        y -= (m <= 2);

        // Step 2: Calculate era (400-year period)
        // Each era is exactly 146,097 days
        const int era = (y >= 0 ? y : y - 399) / 400;

        // Step 3: Calculate year-of-era (0-399)
        // Position within the current 400-year cycle
        const int yoe = static_cast<int>(y - era * 400);

        // Step 4: Calculate day-of-year (0-365)
        // Using formula that accounts for varying month lengths
        const int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;

        // Step 5: Calculate day-of-era (0-146096)
        // Days since start of current 400-year era
        const int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;

        // Step 6: Calculate total days since Unix epoch
        // Combine era count and day-of-era, adjust for epoch offset
        return static_cast<long long>(era) * 146097LL + static_cast<long long>(doe) - 719468LL;
    }

    // ===== CIVIL FROM DAYS ALGORITHM =====

    // Inverse operation: convert days since Unix epoch back to civil date
    // Returns year, month, day in the output parameters
    inline void civilFromDays(long long z, int& y, int& m, int& d)
    {
        // Step 1: Adjust day count back from Unix epoch
        z += 719468;

        // Step 2: Calculate era from day count
        const long long era = (z >= 0 ? z : z - 146096) / 146097;

        // Step 3: Calculate day-of-era (position within 400-year cycle)
        const int doe = static_cast<int>(z - era * 146097);

        // Step 4: Calculate year-of-era (0-399)
        const int yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;

        // Step 5: Calculate year (combine era and year-of-era)
        y = static_cast<int>(yoe) + static_cast<int>(era) * 400;

        // Step 6: Calculate day-of-year from day-of-era
        const int doy = doe - (365*yoe + yoe/4 - yoe/100);

        // Step 7: Calculate month (0-11) from day-of-year
        const int mp = (5*doy + 2) / 153;

        // Step 8: Calculate day (1-31) from day-of-year and month
        d = doy - (153*mp + 2)/5 + 1;

        // Step 9: Convert month from March-based (0-11) to January-based (1-12)
        m = mp + (mp < 10 ? 3 : -9);

        // Step 10: Adjust year if converted month is Jan/Feb (belongs to previous calendar year)
        y += (m <= 2);
    }

    // ===== DAYS IN MONTH HELPER =====

    // Return number of days in given month of given year
    // Accounts for leap years when calculating February
    inline int daysInMonth(int year, int month)
    {
        // Step 1: Static array of days per month (non-leap year)
        static const int dim[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

        // Step 2: Validate month is in range (1-12)
        if (month == 0 || month > 12) return 0;

        // Step 3: Special case February (month 2)
        if (month == 2)
        {
            // Step 3a: Check leap year rules
            // A year is leap if:
            //   - divisible by 400, OR
            //   - divisible by 4 AND not divisible by 100
            bool leap = ((year % 400) == 0) || ((year % 4) == 0 && (year % 100) != 0);

            // Step 3b: Return 29 for leap year, 28 otherwise
            return leap ? 29 : 28;
        }

        // Step 4: Return days for non-February months (lookup table)
        return dim[month - 1];
    }
}

// ======================= LEAP YEAR CHECK =======================

// Determine if this date's year is a leap year
bool Date::isLeapYear() const
{
    // Step 1: Get year value
    int y = m_year;

    // Step 2: Apply leap year rules
    // Leap year if: divisible by 400, OR divisible by 4 AND not by 100
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

// ======================= DATE VALIDITY =======================

// Check if this date is valid in Gregorian calendar
// Returns false for invalid dates like 31-Feb or year <= 0
bool Date::isValid() const
{
    // Step 1: Check year is positive (year 0 not valid in Gregorian calendar)
    if (m_year <= 0) return false;

    // Step 2: Check month is in range (1-12)
    if (m_month < 1 || m_month > 12) return false;

    // Step 3: Check day is positive (at least 1)
    if (m_day < 1) return false;

    // Step 4: Check day doesn't exceed days in this month
    // Use helper to account for leap years in February
    return m_day <= daysInMonth(m_year, m_month);
}

// ======================= DATE ARITHMETIC =======================

// Add n days to this date and return new date
// Positive n advances date, negative n goes back in time
// Uses daysFromCivil/civilFromDays for accurate calculation across month/year boundaries
Date Date::addDays(int n) const
{
    // Step 1: Convert current date to days since Unix epoch
    long long base = daysFromCivil(static_cast<int>(m_year), m_month, m_day);

    // Step 2: Add requested number of days
    long long target = base + static_cast<long long>(n);

    // Step 3: Declare variables to hold converted date components
    int y, mo, d;

    // Step 4: Convert days count back to civil date
    civilFromDays(target, y, mo, d);

    // Step 5: Create and return new Date with result
    return Date(static_cast<int>(d), mo, static_cast<int>(y));
}

// ======================= DATE COMPARISON =======================

// Compare this date to another date
// Returns: -1 if this < other, 0 if equal, 1 if this > other
int Date::compareTo(const Date& other) const
{
    // Step 1: Compare years first (most significant)
    if (m_year != other.m_year)
        return (m_year < other.m_year) ? -1 : 1;

    // Step 2: If years equal, compare months
    if (m_month != other.m_month)
        return (m_month < other.m_month) ? -1 : 1;

    // Step 3: If months equal, compare days
    if (m_day != other.m_day)
        return (m_day < other.m_day) ? -1 : 1;

    // Step 4: All components equal - dates are same
    return 0;
}
