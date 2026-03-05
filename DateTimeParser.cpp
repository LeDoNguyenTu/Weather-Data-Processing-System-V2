#include "DateTimeParser.h"
#include "DateFormatter.h"
#include "TimeFormatter.h"
#include <sstream>

// ======================= PARSE WAST TIMESTAMP =======================

// Parse date and time from combined WAST (date/time) field
// Supports ISO format: "YYYY-MM-DD HH:MM:SS"
// Supports AU format: "D/M/YYYY HH:MM:SS"
bool DateTimeParser::parseWAST(const std::string& field, Date& d, Time& t)
{
    // Step 1: Find space separator between date and time portions
    std::size_t spacePos = field.find(' ');

    // Step 2: Check if space found (indicates date/time split)
    if (spacePos == std::string::npos) return false;

    // Step 3: Extract date portion (before space)
    std::string datePart = field.substr(0, spacePos);

    // Step 4: Extract time portion (after space)
    std::string timePart = field.substr(spacePos + 1);

    // ===== PARSE DATE PORTION =====
    // Step 5: Try parsing date using DateFormatter (supports multiple formats)
    // DateFormatter::tryParse handles: d-MMM-yyyy, d/M/yyyy, d-M-yyyy, yyyyMMdd
    if (!DateFormatter::tryParse(datePart, d))
    {
        // Step 5a: Date parsing failed
        return false;
    }

    // Step 6: Validate parsed date (check calendar validity)
    // Ensures date like 31/02/2015 is rejected
    if (!d.isValid())
    {
        // Step 6a: Date is invalid (e.g., February 30)
        return false;
    }

    // ===== PARSE TIME PORTION =====
    // Step 7: Try parsing time using TimeFormatter
    // TimeFormatter::fromString handles: HH:MM or HH:MM:SS
    try
    {
        // Step 7a: Attempt to parse time string
        t = TimeFormatter::fromString(timePart);

        // Step 7b: Parsing succeeded
        return true;
    }
    catch (...)
    {
        // Step 7c: Time parsing failed (invalid format or out of range)
        return false;
    }
}
