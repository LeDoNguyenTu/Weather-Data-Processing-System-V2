#ifndef DATEFORMATTER_H_INCLUDED
#define DATEFORMATTER_H_INCLUDED

#include <sstream>   // string stream utilities used to build formatted date text (for example, pad with zeros)
#include <string>    // string type for all input and output date text
#include <stdexcept> // exception types thrown on invalid input (for example, invalid_argument)
#include <cctype>    // character classification and case conversion (used to make month names lower case)
#include <iomanip>   // formatting manipulators (setw, setfill) to produce two-digit day and month values
#include "Map.h"     // associative map used to translate month names to month numbers
#include "Date.h"    // Date type used by the formatter interfaces


/**
 * @file DateFormatter.h
 * @brief Formatting and parsing utilities for the Date type.
 *
 * @details
 * This component converts Date values to text and converts text back to Date values.
 * It supports several common input styles. Examples of supported input are:
 * - "18-Sep-2025" which is day hyphen short month name hyphen four digit year
 * - "18/9/2025" which is day slash month slash four digit year
 * - "18-09-2025" which is two digit day hyphen two digit month hyphen four digit year
 * - "20250918" which is four digit year followed by two digit month followed by two digit day
 *
 * For output, you can choose a predefined style or supply a small token pattern.
 * Supported output tokens are:
 * - d    which is day of month without a leading zero
 * - dd   which is day of month with a leading zero when needed
 * - M    which is month number without a leading zero
 * - MM   which is month number with a leading zero when needed
 * - MMM  which is short month name such as Jan, Feb, Mar and so on
 * - yyyy which is four digit year
 *
 * All parsed dates are checked for calendar validity by using Date::isValid.
 * Parsing uses English month names and short month names only.
 */

/**
 * @class DateFormatter
 * @brief Convert Date values to text and parse text into Date values.
 *
 * @details
 * Use the format functions to produce text. Use the parse functions to read text.
 * The parsing functions accept the input styles described in the file overview.
 */
class DateFormatter
{
public:
    /**
     * @enum Style
     * @brief Predefined output styles for the formatter.
     *
     * @details
     * The styles below are short names for common output layouts:
     * - DMY_SLASH means day slash month slash year in the form "dd/MM/yyyy"
     * - DMY_DASH_2 means day hyphen month hyphen year in the form "dd-MM-yyyy"
     * - DMY_MON_SHORT means day hyphen short month name hyphen year in the form "d-MMM-yyyy"
     * - COMPACT_YYYYMMDD means year then month then day without separators in the form "yyyyMMdd"
     */
    enum class Style
    {
        DMY_SLASH,         /**< "dd/MM/yyyy" */
        DMY_DASH_2,        /**< "dd-MM-yyyy" */
        DMY_MON_SHORT,     /**< "d-MMM-yyyy" */
        COMPACT_YYYYMMDD   /**< "yyyyMMdd"   */
    };

    /**
     * @brief Format a date by using a predefined style.
     * @param date The date to format.
     * @param style The style to use. The default is DMY_SLASH.
     * @return A string that contains the formatted date.
     *
     * @details
     * Example: if date is 18 September 2025 and style is DMY_MON_SHORT,
     * the result is the string "18-Sep-2025".
     */
    static std::string format(const Date& date, Style style = Style::DMY_SLASH);

    /**
     * @brief Format a date by using a token pattern.
     * @param date The date to format.
     * @param pattern A pattern string that can contain the tokens d, dd, M, MM, MMM, and yyyy.
     * @return A string that contains the formatted date.
     *
     * @details
     * Characters in the pattern that are not one of the supported tokens are copied as they are.
     * Example: if the pattern is "dd/MMM/yyyy" and the date is 18 September 2025,
     * the result is "18/Sep/2025".
     */
    static std::string format(const Date& date, const std::string& pattern);

    /**
     * @brief Try to parse text into a Date without throwing an exception.
     * @param text The input text to parse.
     * @param outDate This value is set to the parsed date when parsing succeeds. It is not modified when parsing fails.
     * @return True when parsing succeeds. False when parsing fails.
     *
     * @details
     * This function tries each supported input style in turn. It returns a simple true or false result.
     */
    static bool tryParse(const std::string& text, Date& outDate);

    /**
     * @brief Parse text into a Date or throw an exception on failure.
     * @param text The input text to parse.
     * @return The parsed Date.
     * @throws std::invalid_argument When the text does not match any supported style or when it describes an invalid calendar date.
     *
     * @details
     * Use this function when you prefer to handle errors with exceptions instead of checking a boolean return value.
     */
    static Date parseStrict(const std::string& text);

    /**
     * @brief Read one token from a stream and try to parse it as a Date.
     * @param in The input stream to read from.
     * @param outDate This value is set to the parsed date when parsing succeeds. It is not modified when parsing fails.
     * @return True when parsing succeeds. False when parsing fails.
     *
     * @details
     * The function reads a single space separated word from the stream and then tries the supported styles.
     * The function does not put the stream into a failure state. It only consumes the one token that it reads.
     */
    static bool tryParseFromStream(std::istream& in, Date& outDate);

private:
    /**
     * @brief Parse text in the form day separator month separator year, where the separator is slash or hyphen.
     * @param text The input text.
     * @param sep The separator character, which must be slash or hyphen.
     * @param out The result date when parsing succeeds.
     * @return True when parsing succeeds. False when parsing fails.
     */
    static bool parseDMYWithSep(const std::string& text, char sep, Date& out);

    /**
     * @brief Parse text in the form day hyphen short month name hyphen year, or day hyphen full month name hyphen year.
     * @param text The input text with an English month name.
     * @param out The result date when parsing succeeds.
     * @return True when parsing succeeds. False when parsing fails.
     */
    static bool parseDMYMonthName(const std::string& text, Date& out);

    /**
     * @brief Parse text in compact year month day form without separators.
     * @param text The input text that must be exactly eight digits in the order year, month, day.
     * @param out The result date when parsing succeeds.
     * @return True when parsing succeeds. False when parsing fails.
     */
    static bool parseCompactYYYYMMDD(const std::string& text, Date& out);

    /**
     * @brief Convert an English month name or short month name to a month number.
     * @param mon The month text, for example "Jan" or "January". Case is ignored.
     * @return A value from one to twelve when recognized. Returns zero when not recognized.
     */
    static int  monthNameToNumber(const std::string& mon);

    /**
     * @brief Return a lower case copy of the input string by using basic English letter rules.
     * @param s The input string to copy.
     * @return A new string in lower case.
     */
    static std::string toLower(std::string s);

    /**
     * @brief Format an integer as two digits with a leading zero when needed.
     * @param v The value to format. Values from zero to ninety nine are expected.
     * @return A two character string.
     */
    static std::string pad2(int v);
};

#endif // DATEFORMATTER_H_INCLUDED
