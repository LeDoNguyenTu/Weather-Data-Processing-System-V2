#include <cassert>
#include <sstream>
#include <istream>
#include <iostream>
#include "Date.h"
#include "DateFormatter.h"

// ======================= TESTING FRAMEWORK =======================

// Global counters for tracking test results
static int g_pass = 0, g_fail = 0;

// Generic boolean condition check
// Prints result and updates global pass/fail counters
static void check(bool cond, const char* msg)
{
    // Step 1: Print test message with result (1=pass, 0=fail)
    std::cout << msg << " -> " << (cond ? 1 : 0) << "\n";

    // Step 2: Increment pass counter if condition is true
    if (cond) ++g_pass;
    // Step 3: Increment fail counter if condition is false
    else ++g_fail;
}

// ======================= TEST STEPS =========================

// TEST 1: Default and parameterized constructors
static void testDateConstruction()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 1: Default & Parameterized Constructors ---";
    std::cout << "\n====================================================\n";

    // Step 2: Create date with default constructor
    Date defaultDate;

    // Step 3: Print default date
    std::cout << "Default Date: " << defaultDate << std::endl;

    // Step 4: Assert default date is 1/1/1000
    assert(defaultDate.GetDay() == 1 && defaultDate.GetMonth() == 1 && defaultDate.GetYear() == 1000);
    check(true, "Default constructor OK");

    // Step 5: Create date with specific values
    Date specificDate(26, 10, 2025);

    // Step 6: Print specific date
    std::cout << "Specific Date: " << specificDate << std::endl;

    // Step 7: Assert specific date values stored correctly
    assert(specificDate.GetDay() == 26 && specificDate.GetMonth() == 10 && specificDate.GetYear() == 2025);
    check(true, "Parameterized constructor OK");
}

// TEST 2: Set and Get accessor methods
static void testDateSetGet()
{
    // Step 1: Print test section header
    std::cout << "\n=================================\n";
    std::cout << "--- TEST 2: Set/Get Accessors ---";
    std::cout << "\n=================================\n";

    // Step 2: Create date object
    Date date;

    // Step 3: Set day component
    date.SetDay(12);

    // Step 4: Set month component
    date.SetMonth(12);

    // Step 5: Set year component
    date.SetYear(2025);

    // Step 6: Print updated date
    std::cout << "Updated Date: " << date << std::endl;

    // Step 7: Verify all components were set correctly
    if (date.GetDay() == 12 && date.GetMonth() == 12 && date.GetYear() == 2025)
    {
        check(true, "Set/Get works correctly");
    }
    else
    {
        check(false, "Set/Get did not return expected values");
    }
}

// TEST 3: Input/Output operators (extraction >> and insertion <<)
static void testDateInputOutput()
{
    // Step 1: Print test section header
    std::cout << "\n==============================================================\n";
    std::cout << "--- TEST 3: operator>> (parse) and operator<< (format) ---";
    std::cout << "\n==============================================================\n";

    // Step 2: Create input string stream with date in DD/MM/YYYY format
    std::stringstream input("12/03/2025");

    // Step 3: Create default date object
    Date inputDate;

    // Step 4: Parse date from input stream using extraction operator
    input >> inputDate;

    // Step 5: Print parsed date
    std::cout << "Input Date (printed): " << inputDate << std::endl;

    // Step 6: Verify parsed values match expected
    if (inputDate.GetDay() == 12 && inputDate.GetMonth() == 3 && inputDate.GetYear() == 2025)
    {
        check(true, "operator>> parsed correctly");
    }
    else
    {
        check(false, "operator>> did not parse expected values");
    }
}

// TEST 4: Invalid format handling for input operator
static void testInvalidDateFormat_operatorExtraction()
{
    // Step 1: Print test section header
    std::cout << "\n==================================================\n";
    std::cout << "--- TEST 4: operator>> Invalid Format Handling ---";
    std::cout << "\n==================================================\n";

    // Step 2: Create input stream with invalid format
    std::stringstream input("99 December 9999");

    // Step 3: Create date object (stays default if parse fails)
    Date d;

    // Step 4: Attempt to parse invalid format (should set failbit)
    input >> d;

    // Step 5: Check if extraction operator set failbit
    if (input.fail())
    {
        check(true, "operator>> correctly set failbit for invalid format");
    }
    else
    {
        check(false, "operator>> did not set failbit on invalid format");
    }

    // Step 6: Verify date object unchanged after failed parse
    if (d.GetDay() == 1 && d.GetMonth() == 1 && d.GetYear() == 1000)
    {
        check(true, "Date unchanged on parse failure");
    }
    else
    {
        check(false, "Date changed unexpectedly on parse failure");
    }
}

// TEST 5: Leap year detection and date validity checking
static void testLeapYearAndValidity()
{
    // Step 1: Print test section header
    std::cout << "\n==========================================\n";
    std::cout << "--- TEST 5: isLeapYear() and isValid() ---";
    std::cout << "\n==========================================\n";

    // Step 2: Create dates for different year types to test
    Date y2000(29, 2, 2000);   // Century leap year
    Date y1900(29, 2, 1900);   // Century non-leap year
    Date y2024(29, 2, 2024);   // Regular leap year
    Date y2023(29, 2, 2023);   // Regular non-leap year

    // Step 3: Test leap year detection for year 2000 (is leap)
    std::cout << "2000 leap? " << (Date(1,1,2000).isLeapYear() ? "yes" : "no") << std::endl;
    assert(Date(1,1,2000).isLeapYear());
    check(true, "2000 is leap year");

    // Step 4: Test leap year detection for year 1900 (not leap)
    std::cout << "1900 leap? " << (Date(1,1,1900).isLeapYear() ? "yes" : "no") << std::endl;
    assert(!Date(1,1,1900).isLeapYear());
    check(true, "1900 is not leap year");

    // Step 5: Test leap year detection for year 2024 (is leap)
    std::cout << "2024 leap? " << (Date(1,1,2024).isLeapYear() ? "yes" : "no") << std::endl;
    assert(Date(1,1,2024).isLeapYear());
    check(true, "2024 is leap year");

    // Step 6: Test leap year detection for year 2023 (not leap)
    std::cout << "2023 leap? " << (Date(1,1,2023).isLeapYear() ? "yes" : "no") << std::endl;
    assert(!Date(1,1,2023).isLeapYear());
    check(true, "2023 is not leap year");

    // Step 7: Test date validity for 29/02/2000 (valid - leap year)
    std::cout << "Valid 29/02/2000? " << (y2000.isValid() ? "yes" : "no") << std::endl;
    assert(y2000.isValid());
    check(true, "29/02/2000 is valid");

    // Step 8: Test date validity for 29/02/1900 (invalid - not leap year)
    std::cout << "Valid 29/02/1900? " << (y1900.isValid() ? "yes" : "no") << std::endl;
    assert(!y1900.isValid());
    check(true, "29/02/1900 is invalid");

    // Step 9: Test date validity for 29/02/2024 (valid - leap year)
    std::cout << "Valid 29/02/2024? " << (y2024.isValid() ? "yes" : "no") << std::endl;
    assert(y2024.isValid());
    check(true, "29/02/2024 is valid");

    // Step 10: Test date validity for 29/02/2023 (invalid - not leap year)
    std::cout << "Valid 29/02/2023? " << (y2023.isValid() ? "yes" : "no") << std::endl;
    assert(!y2023.isValid());
    check(true, "29/02/2023 is invalid");
}

// TEST 6: Adding days and comparing dates
static void testAddDaysAndCompare()
{
    // Step 1: Print test section header
    std::cout << "\n==========================================\n";
    std::cout << "--- TEST 6: addDays(n) and compareTo() ---";
    std::cout << "\n==========================================\n";

    // Step 2: Create base date (leap year)
    Date d1(28, 2, 2024);

    // Step 3: Add 1 day to get 29/02/2024
    Date d2 = d1.addDays(1);

    // Step 4: Add 2 days to get 01/03/2024
    Date d3 = d1.addDays(2);

    // Step 5: Go back 2 days to get 28/02/2024 again
    Date d4 = d3.addDays(-2);

    // Step 6: Print all dates for verification
    std::cout << "d1: " << d1 << std::endl;
    std::cout << "d1 + 1: " << d2 << " (expect 29 February 2024)" << std::endl;
    std::cout << "d1 + 2: " << d3 << " (expect 01 March 2024)" << std::endl;
    std::cout << "d3 - 2: " << d4 << " (expect 28 February 2024)" << std::endl;

    // Step 7: Verify adding 1 day to 28/02 gives 29/02
    assert(d2.GetDay() == 29 && d2.GetMonth() == 2 && d2.GetYear() == 2024);
    check(true, "addDays(1) works correctly");

    // Step 8: Verify adding 2 days crosses month boundary to March
    assert(d3.GetDay() == 1 && d3.GetMonth() == 3 && d3.GetYear() == 2024);
    check(true, "addDays(2) crosses month boundary");

    // Step 9: Verify negative days works (going back)
    assert(d4.compareTo(d1) == 0);
    check(true, "addDays(-2) reverses correctly");

    // Step 10: Test cross-year addition (31/12/2025 + 1 = 01/01/2026)
    Date end(31, 12, 2025);
    Date nextYear = end.addDays(1);
    std::cout << "31/12/2025 + 1 = " << nextYear << " (expect 01 January 2026)" << std::endl;
    assert(nextYear.GetDay() == 1 && nextYear.GetMonth() == 1 && nextYear.GetYear() == 2026);
    check(true, "addDays crosses year boundary");

    // Step 11: Test compareTo with various date pairs
    Date a(1,1,2020), b(2,1,2020), c(1,2,2020), d(1,1,2021);

    // Step 12: Same date should compare to 0
    assert(a.compareTo(a) == 0);
    check(true, "compareTo same date returns 0");

    // Step 13: Earlier date < later date (same month/year, different day)
    assert(a.compareTo(b) < 0);
    check(true, "compareTo 1/1/2020 < 2/1/2020");

    // Step 14: Later date > earlier date
    assert(b.compareTo(a) > 0);
    check(true, "compareTo 2/1/2020 > 1/1/2020");

    // Step 15: Earlier date < later date (different months)
    assert(a.compareTo(c) < 0);
    check(true, "compareTo 1/1/2020 < 1/2/2020");

    // Step 16: Earlier date < later date (different years)
    assert(c.compareTo(d) < 0);
    check(true, "compareTo 1/2/2020 < 1/1/2021");

    // Step 17: Later date > earlier date (different years)
    assert(d.compareTo(c) > 0);
    check(true, "compareTo 1/1/2021 > 1/2/2020");
}

// TEST 7: DateFormatter parsing and formatting with multiple formats
static void testDateFormatterParseAndFormat()
{
    // Step 1: Print test section header
    std::cout << "\n===========================================================\n";
    std::cout << "--- TEST 7: DateFormatter parse (multi-format) & format ---";
    std::cout << "\n===========================================================\n";

    // Step 2: Parse same date in different input formats
    Date f1 = DateFormatter::parseStrict("25-Sep-2025");   // d-MMM-yyyy
    Date f2 = DateFormatter::parseStrict("25/9/2025");     // d/M/yyyy
    Date f3 = DateFormatter::parseStrict("25-09-2025");    // dd-MM-yyyy
    Date f4 = DateFormatter::parseStrict("20250925");      // yyyyMMdd

    // Step 3: Verify all four inputs parsed to same internal date
    assert(f1.compareTo(f2) == 0 && f2.compareTo(f3) == 0 && f3.compareTo(f4) == 0);
    std::cout << "Parsed dates (all same internal value): \n"
              << " Format 1 (25-Sep-2025): " << f1 << "\n"
              << " Format 2 (25/9/2025): " << f2 << "\n"
              << " Format 3 (25-09-2025): " << f3 << "\n"
              << " Format 4 (20250925): " << f4 << "\n" << std::endl;
    check(true, "Multi-format parsing OK");

    // Step 4: Test formatted output with predefined styles
    std::string s1 = DateFormatter::format(f1, DateFormatter::Style::DMY_SLASH);       // dd/MM/yyyy
    std::string s2 = DateFormatter::format(f1, DateFormatter::Style::DMY_DASH_2);      // dd-MM-yyyy
    std::string s3 = DateFormatter::format(f1, DateFormatter::Style::DMY_MON_SHORT);   // d-MMM-yyyy
    std::string s4 = DateFormatter::format(f1, DateFormatter::Style::COMPACT_YYYYMMDD);// yyyyMMdd

    // Step 5: Print formatted outputs
    std::cout << "DMY_SLASH:      " << s1 << " (expect 25/09/2025)" << std::endl;
    std::cout << "DMY_DASH_2:     " << s2 << " (expect 25-09-2025)" << std::endl;
    std::cout << "DMY_MON_SHORT:  " << s3 << " (expect 25-Sep-2025)" << std::endl;
    std::cout << "COMPACT:        " << s4 << " (expect 20250925)" << std::endl;

    // Step 6: Verify predefined style outputs
    assert(s1 == "25/09/2025");
    check(true, "DMY_SLASH format correct");
    assert(s2 == "25-09-2025");
    check(true, "DMY_DASH_2 format correct");
    assert(s3 == "25-Sep-2025");
    check(true, "DMY_MON_SHORT format correct");
    assert(s4 == "20250925");
    check(true, "COMPACT_YYYYMMDD format correct");

    // Step 7: Test custom pattern formatting
    std::string s5 = DateFormatter::format(f1, "dd/MMM/yyyy");   // Custom pattern
    std::string s6 = DateFormatter::format(f1, "dd MMMM yyyy");  // Full month name

    std::cout << "Pattern dd/MMM/yyyy: " << s5 << " (expect 25/Sep/2025)" << std::endl;
    std::cout << "Pattern dd MMMM yyyy: " << s6 << " (expect 25 September 2025)" << std::endl;

    // Step 8: Verify custom pattern formatting
    assert(s5 == "25/Sep/2025");
    check(true, "Custom pattern dd/MMM/yyyy OK");
    assert(s6 == "25 September 2025");
    check(true, "Custom pattern dd MMMM yyyy OK");

    // Step 9: Test tryParse with success cases
    Date out;
    bool ok1 = DateFormatter::tryParse("1-Feb-2020", out);      // Should succeed
    bool ok2 = DateFormatter::tryParse("01/02/2020", out);      // Should succeed
    bool ok3 = DateFormatter::tryParse("01-02-2020", out);      // Should succeed
    bool ok4 = DateFormatter::tryParse("20200201", out);        // Should succeed
    bool bad = DateFormatter::tryParse("Feb/01/2020", out);     // Should fail (unsupported)

    // Step 10: Verify tryParse returns correct status
    assert(ok1 && ok2 && ok3 && ok4 && !bad);
    check(true, "tryParse success/failure paths OK");
}

// TEST 8: DateFormatter parseStrict with invalid input
static void testDateFormatterParseStrictInvalid()
{
    // Step 1: Print test section header
    std::cout << "\n========================================================\n";
    std::cout << "--- TEST 8: DateFormatter::parseStrict invalid input ---";
    std::cout << "\n========================================================\n";

    // Step 2: Attempt to parse unsupported format
    try
    {
        // Step 3: Try to parse with slash separator (unsupported pattern)
        (void)DateFormatter::parseStrict("2025/09/18");

        // Step 4: If we get here, exception was not thrown
        check(false, "Expected exception not thrown for invalid format");
    }
    catch (const std::invalid_argument&)
    {
        // Step 5: Expected exception was thrown
        check(true, "parseStrict threw invalid_argument as expected");
    }
}

// ======================= SUMMARY =========================

// Print test results
static void printTotalsAndExitCode(int& exitCodeOut)
{
    // Step 1: Print total number of passing tests
    std::cout << "\nTOTAL PASSED: " << g_pass << "\n";

    // Step 2: Print total number of failing tests
    std::cout << "TOTAL FAILED: " << g_fail << "\n";

    // Step 3: Set exit code: 0 if all tests passed, 1 if any failed
    exitCodeOut = (g_fail == 0) ? 0 : 1;
}

// ======================= MAIN TEST DRIVER =========================

// Main entry point - runs all Date unit tests
int main()
{
    // Step 1: Run Test 1 - constructors
    testDateConstruction();

    // Step 2: Run Test 2 - set/get accessors
    testDateSetGet();

    // Step 3: Run Test 3 - input/output operators
    testDateInputOutput();

    // Step 4: Run Test 4 - invalid input handling
    testInvalidDateFormat_operatorExtraction();

    // Step 5: Run Test 5 - leap year and validity
    testLeapYearAndValidity();

    // Step 6: Run Test 6 - add days and compare
    testAddDaysAndCompare();

    // Step 7: Run Test 7 - formatter parsing and formatting
    testDateFormatterParseAndFormat();

    // Step 8: Run Test 8 - parseStrict invalid input
    testDateFormatterParseStrictInvalid();

    // Step 9: Print summary and exit code
    int code = 0;
    printTotalsAndExitCode(code);

    // Step 10: Print completion message
    std::cout << "\nAll tests executed.\n";

    // Step 11: Return exit code (0=success, 1=failure)
    return code;
}
