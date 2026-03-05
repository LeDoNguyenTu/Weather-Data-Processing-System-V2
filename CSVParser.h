#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <string>
#include "StringList.h"

/**
 * @class CSVParser
 * @brief Utility class for parsing CSV data.
 * @details Provides static methods for splitting CSV lines, parsing numeric fields,
 *          and locating columns in header rows. All methods are static - no instances needed.
 *
 * Responsibilities:
 * - Split CSV lines handling quoted fields
 * - Parse float fields with offline marker detection
 * - Locate required columns in header row
 * - Trim whitespace from strings
 */
class CSVParser
{
public:
    /**
     * @brief Split a CSV line into fields, handling quoted strings.
     * @param line CSV line to parse
     * @param out StringList to receive parsed fields
     *
     * @details Parses comma-separated values, respecting quoted strings
     * which may contain commas. Quotes are stripped from output fields.
     * Example: "John,\"Smith, Jr\",42" -> ["John", "Smith, Jr", "42"]
     */
    static void splitCSV(const std::string& line, StringList& out);

    /**
     * @brief Parse a numeric field, rejecting offline markers.
     * @param s Input field string
     * @param out Parsed float value (output)
     * @return true if valid number parsed, false if offline marker or parse error
     *
     * @details Rejects common offline indicators: "N/A", "NA", "NULL", "-", etc.
     * Only accepts valid numeric strings. Uses std::strtof for conversion.
     */
    static bool parseFloatField(const std::string& s, float& out);

    /**
     * @brief Locate required CSV columns by header row.
     * @param header Parsed header row StringList
     * @param idxWAST Column index of WAST field (output)
     * @param idxS Column index of S (wind speed) (output)
     * @param idxT Column index of T (temperature) (output)
     * @param idxSR Column index of SR (solar radiation) (output)
     * @return true if all required columns found, false if any missing
     *
     * @details Searches header for column names (case-insensitive).
     * Required columns: WAST, S, T, SR. Sets output indices to -1 if not found.
     */
    static bool findColumns(const StringList& header,
                           int& idxWAST, int& idxS, int& idxT, int& idxSR);

    /**
     * @brief Remove leading and trailing whitespace from string.
     * @param s Input string
     * @return New string with whitespace removed from both ends
     *
     * @details Trims ASCII whitespace characters (space, tab, newline, etc.).
     * Original string unchanged - returns new trimmed copy.
     */
    static std::string trim(const std::string& s);
};

#endif // CSV_PARSER_H
