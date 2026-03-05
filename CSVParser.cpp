#include "CSVParser.h"
#include <cctype>
#include <cstdlib>

// ======================= TRIM WHITESPACE =======================

// Remove leading and trailing ASCII whitespace from string
// Returns new string with whitespace removed (original unchanged)
std::string CSVParser::trim(const std::string& s)
{
    // Step 1: Initialize start index a = 0
    std::size_t a = 0;

    // Step 2: Initialize end index b = string length
    std::size_t b = s.size();

    // Step 3: Move start index forward past leading whitespace
    // isspace checks for space, tab, newline, carriage return, etc.
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;

    // Step 4: Move end index backward past trailing whitespace
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;

    // Step 5: Return substring from a to b (excluding whitespace)
    // substr(start, length) extracts middle portion without whitespace
    return s.substr(a, b - a);
}

// ======================= SPLIT CSV LINE =======================

// Split CSV line into fields, handling quoted fields correctly
// Parses comma-separated line, respecting quoted strings (which may contain commas)
void CSVParser::splitCSV(const std::string& line, StringList& out)
{
    // Step 1: Clear output StringList to start fresh
    out.clear();

    // Step 2: Declare variables for parsing state
    std::string field;          // Current field being built
    bool inQuotes = false;      // Track if inside quoted string
    std::size_t i = 0;          // Current position in line

    // Step 3: Loop through each character in line
    while (i < line.size())
    {
        // Step 3a: Get current character
        char c = line[i];

        // Step 4: Handle quote character
        if (c == '"')
        {
            // Step 4a: Toggle quote state (entering or leaving quoted section)
            inQuotes = !inQuotes;
            ++i;
            continue;
        }

        // Step 5: Handle comma separator
        if (c == ',' && !inQuotes)
        {
            // Step 5a: Comma outside quotes marks end of field
            // Add current field to output list
            out.push_back(field);

            // Step 5b: Clear field for next value
            field.clear();
            ++i;
            continue;
        }

        // Step 6: Regular character - add to current field
        field += c;
        ++i;
    }

    // Step 7: Add final field after loop completes
    // (no trailing comma, so last field not yet added)
    out.push_back(field);
}

// ======================= PARSE FLOAT FIELD =======================

// Parse numeric field, rejecting offline/invalid markers
// Converts string to float, but rejects offline indicators
bool CSVParser::parseFloatField(const std::string& s, float& out)
{
    // Step 1: Trim whitespace from input field
    std::string trimmed = trim(s);

    // Step 2: Check if field is empty after trimming
    if (trimmed.empty()) return false;

    // Step 3: Check for common offline markers (case-sensitive)
    // These indicate sensor was offline or data unavailable
    if (trimmed == "N/A" || trimmed == "NA" ||
        trimmed == "NULL" || trimmed == "-" ||
        trimmed == "n/a" || trimmed == "null")
    {
        return false;  // Reject offline markers
    }

    // Step 4: Attempt to parse as float using strtof
    // strtof converts string to float, sets endptr to first non-numeric char
    char* endptr = nullptr;
    float value = std::strtof(trimmed.c_str(), &endptr);

    // Step 5: Check if entire string was consumed (valid number)
    // If endptr points to null terminator, entire string was numeric
    if (endptr && *endptr == '\0')
    {
        // Step 5a: Valid number - store in output parameter
        out = value;
        return true;
    }

    // Step 6: Parsing failed - string contains non-numeric characters
    return false;
}

// ======================= FIND REQUIRED COLUMNS =======================

// Locate required CSV columns by searching header row
// Returns true if all required columns found, false if any missing
bool CSVParser::findColumns(const StringList& header,
                           int& idxWAST, int& idxS, int& idxT, int& idxSR)
{
    // Step 1: Initialize all indices to -1 (not found)
    idxWAST = -1;
    idxS = -1;
    idxT = -1;
    idxSR = -1;

    // Step 2: Loop through each column in header row
    for (std::size_t i = 0; i < header.size(); ++i)
    {
        // Step 2a: Get column name and convert to uppercase for comparison
        std::string col = header[i];

        // Step 2b: Convert to uppercase for case-insensitive comparison
        for (std::size_t j = 0; j < col.size(); ++j)
        {
            col[j] = static_cast<char>(std::toupper(static_cast<unsigned char>(col[j])));
        }

        // Step 3: Check if column matches required names
        // WAST = timestamp (Western Australian Standard Time)
        if (col == "WAST") idxWAST = static_cast<int>(i);

        // S = wind speed in m/s
        else if (col == "S") idxS = static_cast<int>(i);

        // T = temperature in Celsius
        else if (col == "T") idxT = static_cast<int>(i);

        // SR = solar radiation in W/m²
        else if (col == "SR") idxSR = static_cast<int>(i);
    }

    // Step 4: Return true only if all four required columns were found
    // All indices must be >= 0 (not -1)
    return (idxWAST >= 0 && idxS >= 0 && idxT >= 0 && idxSR >= 0);
}
