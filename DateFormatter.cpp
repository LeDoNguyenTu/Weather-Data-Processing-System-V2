#include "DateFormatter.h"
#include "Map.h"

// ======================= LOCAL HELPER FUNCTIONS =======================

// Trim leading and trailing ASCII whitespace from a string
// Returns new string with whitespace removed (original unchanged)
static std::string trim_copy(const std::string& s)
{
    // Step 1: Initialize start index a = 0
    std::size_t a = 0;

    // Step 2: Initialize end index b = string length
    std::size_t b = s.size();

    // Step 3: Move start index forward past leading whitespace
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;

    // Step 4: Move end index backward past trailing whitespace
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;

    // Step 5: Return substring from a to b (excluding whitespace)
    return s.substr(a, b - a);
}

// Check if all characters in string are decimal digits
// Returns false for empty string
static bool all_digits(const std::string& s)
{
    // Step 1: Return false if string is empty
    if (s.empty()) return false;

    // Step 2: Loop through each character
    for (std::size_t i = 0; i < s.size(); ++i)
    {
        // Step 2a: Check if character is NOT a digit
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    }

    // Step 3: All characters are digits
    return true;
}

// Parse unsigned integer from digit string
// Caller must have already validated that string contains only digits
static unsigned to_uint(const std::string& s)
{
    // Step 1: Use std::stoul to convert string to unsigned long
    // Step 2: Cast to unsigned int (safe because we validated digits)
    // Step 3: Return the integer value
    return static_cast<unsigned>(std::stoul(s));
}

// ======================= CASE CONVERSION =======================

// Convert string to lowercase (ASCII only)
// Returns converted string (original unchanged)
std::string DateFormatter::toLower(std::string s)
{
    // Step 1: Loop through each character in string
    for (std::size_t i = 0; i < s.size(); ++i)
    {
        // Step 1a: Convert character to lowercase
        // Step 1b: Cast through unsigned char for safety with extended ASCII
        s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    }

    // Step 2: Return modified string
    return s;
}

// ======================= MONTH NAME TO NUMBER =======================

// Convert month name to number (1-12)
// Supports both short (Jan, Jan) and long (January) names
// Case-insensitive; returns 0 if month name not recognized
int DateFormatter::monthNameToNumber(const std::string& monIn)
{
    // Step 1: Create static map of month name -> number mappings
    // Includes both short and long forms for flexibility
    static Map<std::string, int> monthMap;

    // Step 2: Initialize monthMap on first call (check if empty)
    static bool initialized = false;
    if (!initialized)
    {
        // January
        monthMap.put("jan", 1);
        monthMap.put("january", 1);
        // February
        monthMap.put("feb", 2);
        monthMap.put("february", 2);
        // March
        monthMap.put("mar", 3);
        monthMap.put("march", 3);
        // April
        monthMap.put("apr", 4);
        monthMap.put("april", 4);
        // May
        monthMap.put("may", 5);
        // June
        monthMap.put("jun", 6);
        monthMap.put("june", 6);
        // July
        monthMap.put("jul", 7);
        monthMap.put("july", 7);
        // August
        monthMap.put("aug", 8);
        monthMap.put("august", 8);
        // September (including Sept variant)
        monthMap.put("sep", 9);
        monthMap.put("sept", 9);
        monthMap.put("september", 9);
        // October
        monthMap.put("oct", 10);
        monthMap.put("october", 10);
        // November
        monthMap.put("nov", 11);
        monthMap.put("november", 11);
        // December
        monthMap.put("dec", 12);
        monthMap.put("december", 12);

        initialized = true;
    }

    // Step 2: Convert input to lowercase for case-insensitive lookup
    std::string k = toLower(monIn);

    // Step 3: Search for month name in map
    if (monthMap.contains(k))
    {
        return monthMap.get(k);
    }

    // Step 4: Return corresponding number if found, 0 if not found
    return 0;
}

// ======================= PADDING HELPER =======================

// Format integer with zero-padding to 2 digits (e.g., 5 -> "05")
std::string DateFormatter::pad2(int v)
{
    // Step 1: Create output string stream
    std::ostringstream os;

    // Step 2: Set field width to 2
    os << std::setw(2);

    // Step 3: Set fill character to '0'
    os << std::setfill('0');

    // Step 4: Output the integer (will be zero-padded)
    os << v;

    // Step 5: Return formatted string
    return os.str();
}

// ======================= PARSING: D/M/YYYY FORMAT =======================

// Parse date with separator (/ or -)
// Format: d/m/yyyy or dd/mm/yyyy (1-2 digits for day/month, exactly 4 for year)
bool DateFormatter::parseDMYWithSep(const std::string& text, char sep, Date& out)
{
    // Step 1: Validate separator is / or -
    if (sep != '/' && sep != '-') return false;

    // Step 2: Trim whitespace from input
    std::string s = trim_copy(text);

    // Step 3: Find first separator
    std::size_t p1 = s.find(sep);
    if (p1 == std::string::npos) return false;

    // Step 4: Find second separator
    std::size_t p2 = s.find(sep, p1 + 1);
    if (p2 == std::string::npos) return false;

    // Step 5: Ensure no third separator (validate exactly 2 separators)
    if (s.find(sep, p2 + 1) != std::string::npos) return false;

    // Step 6: Extract day string (before first separator)
    std::string dayStr   = trim_copy(s.substr(0, p1));

    // Step 7: Extract month string (between separators)
    std::string monthStr = trim_copy(s.substr(p1 + 1, p2 - (p1 + 1)));

    // Step 8: Extract year string (after second separator)
    std::string yearStr  = trim_copy(s.substr(p2 + 1));

    // Step 9: Validate day is 1-2 digits
    if (!(dayStr.size() >= 1 && dayStr.size() <= 2 && all_digits(dayStr))) return false;

    // Step 10: Validate month is 1-2 digits
    if (!(monthStr.size() >= 1 && monthStr.size() <= 2 && all_digits(monthStr))) return false;

    // Step 11: Validate year is exactly 4 digits
    if (!(yearStr.size() == 4 && all_digits(yearStr))) return false;

    // Step 12: Convert strings to unsigned integers
    unsigned d  = to_uint(dayStr);
    unsigned mo = to_uint(monthStr);
    unsigned y  = to_uint(yearStr);

    // Step 13: Create Date candidate with parsed values
    Date candidate(d, mo, y);

    // Step 14: Check if date is valid (e.g., not 31/02/2025)
    if (!candidate.isValid()) return false;

    // Step 15: Date is valid - store in output and return true
    out = candidate;
    return true;
}

// ======================= PARSING: D-MMM-YYYY FORMAT =======================

// Parse date with month name
// Format: d-MMM-yyyy or d-MMMM-yyyy (case-insensitive month)
// Examples: 18-Sep-2025, 3-September-2025
bool DateFormatter::parseDMYMonthName(const std::string& text, Date& out)
{
    // Step 1: Trim whitespace from input
    std::string s = trim_copy(text);

    // Step 2: Find first hyphen (separator between day and month)
    std::size_t p1 = s.find('-');
    if (p1 == std::string::npos) return false;

    // Step 3: Find second hyphen (separator between month and year)
    std::size_t p2 = s.find('-', p1 + 1);
    if (p2 == std::string::npos) return false;

    // Step 4: Ensure no third hyphen (month names don't contain hyphens)
    if (s.find('-', p2 + 1) != std::string::npos) return false;

    // Step 5: Extract day string (before first hyphen)
    std::string dayStr   = trim_copy(s.substr(0, p1));

    // Step 6: Extract month string (between hyphens)
    std::string monthStr = trim_copy(s.substr(p1 + 1, p2 - (p1 + 1)));

    // Step 7: Extract year string (after second hyphen)
    std::string yearStr  = trim_copy(s.substr(p2 + 1));

    // Step 8: Validate day is 1-2 digits
    if (!(dayStr.size() >= 1 && dayStr.size() <= 2 && all_digits(dayStr))) return false;

    // Step 9: Validate year is exactly 4 digits
    if (!(yearStr.size() == 4 && all_digits(yearStr))) return false;

    // Step 10: Validate month string is not empty
    if (monthStr.empty()) return false;

    // Step 11: Validate all characters in month are alphabetic
    for (std::size_t i = 0; i < monthStr.size(); ++i)
    {
        if (!std::isalpha(static_cast<unsigned char>(monthStr[i]))) return false;
    }

    // Step 12: Convert month name to number (1-12)
    int moNum = monthNameToNumber(monthStr);

    // Step 13: Check if month name was recognized (0 = unrecognized)
    if (moNum == 0) return false;

    // Step 14: Convert day and year strings to integers
    int d  = to_uint(dayStr);
    int mo = static_cast<int>(moNum);
    int y  = to_uint(yearStr);

    // Step 15: Create Date candidate
    Date candidate(d, mo, y);

    // Step 16: Validate date (e.g., not 31-Sep-2025)
    if (!candidate.isValid()) return false;

    // Step 17: Date is valid - store in output and return true
    out = candidate;
    return true;
}

// ======================= PARSING: COMPACT YYYYMMDD FORMAT =======================

// Parse compact date format
// Format: yyyyMMdd (exactly 8 digits, no separators)
// Example: 20250918 = September 18, 2025
bool DateFormatter::parseCompactYYYYMMDD(const std::string& text, Date& out)
{
    // Step 1: Trim whitespace from input
    std::string s = trim_copy(text);

    // Step 2: Check if exactly 8 characters and all digits
    if (s.size() != 8 || !all_digits(s)) return false;

    // Step 3: Extract year (first 4 digits)
    std::string yStr  = s.substr(0, 4);

    // Step 4: Extract month (next 2 digits)
    std::string mStr  = s.substr(4, 2);

    // Step 5: Extract day (last 2 digits)
    std::string dStr  = s.substr(6, 2);

    // Step 6: Convert strings to unsigned integers
    unsigned y  = to_uint(yStr);
    unsigned mo = to_uint(mStr);
    unsigned d  = to_uint(dStr);

    // Step 7: Create Date candidate with parsed values
    Date candidate(d, mo, y);

    // Step 8: Validate date is valid
    if (!candidate.isValid()) return false;

    // Step 9: Date is valid - store in output and return true
    out = candidate;
    return true;
}

// ======================= PARSING: TRY MULTIPLE FORMATS =======================

// Attempt to parse date string using any supported format
// Tries formats in order: name, slash, dash, compact
bool DateFormatter::tryParse(const std::string& text, Date& outDate)
{
    // Step 1: Try D-MMM-YYYY format first (most specific)
    if (parseDMYMonthName(text, outDate)) return true;

    // Step 2: Try D/M/YYYY format
    if (parseDMYWithSep(text, '/', outDate)) return true;

    // Step 3: Try D-M-YYYY format
    if (parseDMYWithSep(text, '-', outDate)) return true;

    // Step 4: Try YYYYMMDD compact format
    if (parseCompactYYYYMMDD(text, outDate)) return true;

    // Step 5: All formats failed
    return false;
}

// ======================= STRICT PARSING =======================

// Parse date string, throwing exception if format not supported
// Used when caller requires date parsing to succeed or fail decisively
Date DateFormatter::parseStrict(const std::string& text)
{
    // Step 1: Create Date object to hold result
    Date d;

    // Step 2: Try parsing with flexible format
    if (tryParse(text, d)) return d;

    // Step 3: Parsing failed - throw exception with descriptive message
    throw std::invalid_argument("DateFormatter::parseStrict: unsupported date format: \"" + text + "\"");
}

// ======================= FORMATTING: PREDEFINED STYLES =======================

// Format Date using one of the predefined output styles
std::string DateFormatter::format(const Date& date, Style style)
{
    // Step 1: Extract date components
    unsigned d = date.GetDay();
    unsigned m = date.GetMonth();
    unsigned y = date.GetYear();

    // Step 2: Format based on selected style
    switch (style)
    {
    case Style::DMY_SLASH:
        return pad2(d) + "/" + pad2(m) + "/" + std::to_string(y);

    case Style::DMY_DASH_2:
        return pad2(d) + "-" + pad2(m) + "-" + std::to_string(y);

    case Style::DMY_MON_SHORT:
    {
        static const char* MMM[12] =
        {
            "Jan","Feb","Mar","Apr","May","Jun",
            "Jul","Aug","Sep","Oct","Nov","Dec"
        };

        const char* mon = (m >= 1 && m <= 12) ? MMM[m - 1] : "???";

        std::ostringstream os;
        os << d << "-" << mon << "-" << y;
        return os.str();
    }
    case Style::COMPACT_YYYYMMDD:
        return std::to_string(y) + pad2(m) + pad2(d);

    default:
        return pad2(d) + "/" + pad2(m) + "/" + std::to_string(y);
    }
}

// ======================= FORMATTING: CUSTOM PATTERNS =======================

// Format Date using custom token pattern
// Tokens: yyyy (year), MMMM (full month), MMM (short month), dd, MM, d, M, or literals
std::string DateFormatter::format(const Date& date, const std::string& pattern)
{
    static const char* MMM[12]  = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    static const char* MMMM[12] = {"January","February","March","April","May","June","July","August",
                                   "September","October","November","December"
                                  };

    std::ostringstream out;

    for (std::size_t i = 0; i < pattern.size(); )
    {
        if (pattern.compare(i, 4, "yyyy") == 0)
        {
            out << date.GetYear();
            i += 4;
        }
        else if (pattern.compare(i, 4, "MMMM") == 0)
        {
            unsigned m = date.GetMonth();
            out << ((m>=1 && m<=12) ? MMMM[m-1] : "????");
            i += 4;
        }
        else if (pattern.compare(i, 3, "MMM") == 0)
        {
            unsigned m = date.GetMonth();
            out << ((m>=1 && m<=12) ? MMM[m-1] : "???");
            i += 3;
        }
        else if (pattern.compare(i, 2, "dd") == 0)
        {
            out << pad2(date.GetDay());
            i += 2;
        }
        else if (pattern.compare(i, 2, "MM") == 0)
        {
            out << pad2(date.GetMonth());
            i += 2;
        }
        else if (pattern[i] == 'd')
        {
            out << date.GetDay();
            ++i;
        }
        else if (pattern[i] == 'M')
        {
            out << date.GetMonth();
            ++i;
        }
        else
        {
            out << pattern[i++];
        }
    }

    return out.str();
}

// ======================= STREAM INPUT PARSING =======================

// Parse date from input stream
// Reads one whitespace-delimited token and tries to parse as date
// Returns true if successful, false if parsing fails
bool DateFormatter::tryParseFromStream(std::istream& in, Date& outDate)
{
    in >> std::ws;

    std::string token;

    if (!(in >> token)) return false;

    Date d;

    if (tryParse(token, d))
    {
        outDate = d;
        return true;
    }

    return false;
}
