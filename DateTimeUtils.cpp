#include "DateTimeUtils.h"

// ======================= MONTH/YEAR FILTER TEST =======================

// Test whether a reading belongs to a specific month and year
// Used to filter readings during AVL tree traversal
bool DateTimeUtils::isMonthYear(const WeatherReading& r, int year, int month)
{
    // Step 1: Compare year component
    // Step 2: Compare month component
    // Step 3: Return true only if both match
    return (r.d.GetYear() == year && r.d.GetMonth() == month);
}

// ======================= MONTH NAME LOOKUP =======================

// Retrieve English name of a month for display/reporting
// Returns pointer to static string (valid month names)
const char* DateTimeUtils::monthName(int m)
{
    // Step 1: Define static array of month names
    // Index 0 unused; indices 1-12 map to January-December
    static const char* names[13] = {
        "?",          // 0 - invalid
        "January",    // 1
        "February",   // 2
        "March",      // 3
        "April",      // 4
        "May",        // 5
        "June",       // 6
        "July",       // 7
        "August",     // 8
        "September",  // 9
        "October",    // 10
        "November",   // 11
        "December"    // 12
    };

    // Step 2: Validate month is in range [1, 12]
    if (m < 1 || m > 12)
    {
        // Step 2a: Invalid month - return "?"
        return names[0];
    }

    // Step 3: Return month name at corresponding index
    return names[m];
}

// ======================= TIMESTAMP KEY GENERATION =======================

// Generate unique 64-bit key combining date and time for deduplication
// Used as key in Map<unsigned long long, bool> to detect duplicate timestamps
unsigned long long DateTimeUtils::timestampKey(const Date& d, const Time& t)
{
    // Step 1: Extract date components
    unsigned long long year = static_cast<unsigned long long>(d.GetYear());
    unsigned long long month = static_cast<unsigned long long>(d.GetMonth());
    unsigned long long day = static_cast<unsigned long long>(d.GetDay());

    // Step 2: Extract time components
    unsigned long long hour = static_cast<unsigned long long>(t.hour());
    unsigned long long minute = static_cast<unsigned long long>(t.minute());
    unsigned long long second = static_cast<unsigned long long>(t.second());

    // Step 3: Combine into single 64-bit key using bit shifting
    // Year: bits 40-63 (24 bits, supports years 0-16777215)
    // Month: bits 36-39 (4 bits, supports 0-15)
    // Day: bits 31-35 (5 bits, supports 0-31)
    // Hour: bits 26-30 (5 bits, supports 0-31)
    // Minute: bits 20-25 (6 bits, supports 0-63)
    // Second: bits 14-19 (6 bits, supports 0-63)
    // Remaining bits unused (allows for future expansion)
    unsigned long long key =
        (year << 40) |      // Shift year to upper bits
        (month << 36) |     // Shift month
        (day << 31) |       // Shift day
        (hour << 26) |      // Shift hour
        (minute << 20) |    // Shift minute
        (second << 14);     // Shift second

    // Step 4: Return combined key
    // Each unique date/time combination produces unique key
    return key;
}
