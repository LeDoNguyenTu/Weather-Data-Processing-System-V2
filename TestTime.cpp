#include <cassert>
#include <iostream>
#include "Time.h"
#include "TimeFormatter.h"

// ======================= TEST 1: Constructors & TimeFormatter::format() =======================

// Test parameterized and default constructors, plus format() formatting
static void testConstructionAndToString()
{
    // Step 1: Print test section header
    std::cout << "\n=========================================\n";
    std::cout << "--- TEST 1: Constructors & format()   ---";
    std::cout << "\n=========================================\n";

    // Step 2: Create Time object with specific values (23:59:58)
    Time t(23, 59, 58);

    // Step 3: Convert to string using TimeFormatter and print for verification
    std::cout << "TimeFormatter::format(t): " << TimeFormatter::format(t) << " (expect 23:59:58)\n";

    // Step 4: Assert that string output matches expected format
    assert(TimeFormatter::format(t) == "23:59:58");

    // Step 5: Create Time with default constructor (should be 00:00:00)
    Time z;

    // Step 6: Print and verify default time
    std::cout << "default: " << TimeFormatter::format(z) << " (expect 00:00:00)\n";

    // Step 7: Assert default time is midnight
    assert(TimeFormatter::format(z) == "00:00:00");

    // Step 8: Print test completion message
    std::cout << "  Constructors & format() OK\n";
}

// ======================= TEST 2: TimeFormatter::fromString() Variants =======================

// Test string parsing with different formats (HH:MM and HH:MM:SS)
static void testFromStringVariants()
{
    // Step 1: Print test section header
    std::cout << "\n=================================\n";
    std::cout << "--- TEST 2: fromString() forms ---";
    std::cout << "\n=================================\n";

    // ===== Format: HH:MM (seconds default to 0) =====
    // Step 2: Parse "07:05" format (no seconds specified)
    Time a = TimeFormatter::fromString("07:05");

    // Step 3: Convert back to string and print
    std::cout << "fromString(\"07:05\"): " << TimeFormatter::format(a) << " (expect 07:05:00)\n";

    // Step 4: Assert parsed time has seconds defaulted to 0
    assert(a == Time(7, 5, 0));

    // ===== Format: HH:MM:SS (all components specified) =====
    // Step 5: Parse full format "00:00:00"
    Time b = TimeFormatter::fromString("00:00:00");

    // Step 6: Print and verify
    std::cout << "fromString(\"00:00:00\"): " << TimeFormatter::format(b) << " (expect 00:00:00)\n";

    // Step 7: Assert boundary case (midnight)
    assert(b == Time(0, 0, 0));

    // ===== Edge case: Single-digit fields =====
    // Step 8: Parse single-digit components "7:5" (sscanf handles this)
    Time c = TimeFormatter::fromString("7:5");

    // Step 9: Print result - should pad to two digits
    std::cout << "fromString(\"7:5\"): " << TimeFormatter::format(c) << " (expect 07:05:00)\n";

    // Step 10: Assert single-digit parsing works correctly
    assert(c == Time(7, 5, 0));

    // ===== Edge case: Surrounding whitespace =====
    // Step 11: Parse string with leading/trailing whitespace
    Time d = TimeFormatter::fromString("  9:08:07  ");

    // Step 12: Print result
    std::cout << "fromString(\"  9:08:07  \"): " << TimeFormatter::format(d) << " (expect 09:08:07)\n";

    // Step 13: Assert whitespace is properly handled
    assert(d == Time(9, 8, 7));

    // Step 14: Print test completion
    std::cout << "  fromString() variants OK\n";
}

// ======================= TEST 3: Range Checking =======================

// Test that constructors and set() reject out-of-range values
static void testRangeCheckingCtorAndSet()
{
    // Step 1: Print test section header
    std::cout << "\n=================================================\n";
    std::cout << "--- TEST 3: Range checks (ctor and set())     ---";
    std::cout << "\n=================================================\n";

    // ===== Test 1: Constructor rejects hour >= 24 =====
    // Step 2: Initialize flag to track if exception was thrown
    bool threw = false;

    // Step 3: Attempt to create Time with invalid hour (24)
    try
    {
        Time bad(24, 0, 0);
    }
    // Step 4: Catch out_of_range exception
    catch (const std::out_of_range&)
    {
        threw = true;
    }

    // Step 5: Print result
    std::cout << "ctor(24,0,0) throws out_of_range? " << (threw ? "yes" : "no") << "\n";

    // Step 6: Assert exception was indeed thrown
    assert(threw);

    // ===== Test 2: set() rejects minute >= 60 =====
    // Step 7: Create valid Time object
    Time t(12, 0, 0);

    // Step 8: Reset exception flag
    threw = false;

    // Step 9: Attempt to set invalid minute (60)
    try
    {
        t.set(12, 60, 0);
    }
    // Step 10: Catch out_of_range exception
    catch (const std::out_of_range&)
    {
        threw = true;
    }

    // Step 11: Print result
    std::cout << "set(12,60,0) throws out_of_range? " << (threw ? "yes" : "no") << "\n";

    // Step 12: Assert exception was thrown
    assert(threw);

    // ===== Test 3: set() rejects second >= 60 =====
    // Step 13: Reset exception flag
    threw = false;

    // Step 14: Attempt to set invalid second (60)
    try
    {
        t.set(12, 59, 60);
    }
    // Step 15: Catch out_of_range exception
    catch (const std::out_of_range&)
    {
        threw = true;
    }

    // Step 16: Print result
    std::cout << "set(12,59,60) throws out_of_range? " << (threw ? "yes" : "no") << "\n";

    // Step 17: Assert exception was thrown
    assert(threw);

    // ===== Test 4: set() accepts boundary value 23:59:59 =====
    // Step 18: Attempt to set maximum valid values
    t.set(23, 59, 59);

    // Step 19: Convert to string and print
    std::cout << "set(23,59,59) -> " << TimeFormatter::format(t) << " (expect 23:59:59)\n";

    // Step 20: Assert time was set correctly (no exception)
    assert(TimeFormatter::format(t) == "23:59:59");

    // Step 21: Print test completion
    std::cout << "  Range checks OK\n";
}

// ======================= TEST 4: Equality Operators =======================

// Test == and != operators for Time comparison
static void testEqualityAndInequality()
{
    // Step 1: Print test section header
    std::cout << "\n=====================================\n";
    std::cout << "--- TEST 4: == and != operators   ---";
    std::cout << "\n=====================================\n";

    // Step 2: Create three Time objects (two equal, one different)
    Time a(1,2,3), b(1,2,3), c(1,2,4);

    // Step 3: Print the three times for reference
    std::cout << "a=" << TimeFormatter::format(a) << ", b=" << TimeFormatter::format(b)
              << ", c=" << TimeFormatter::format(c) << "\n";

    // Step 4: Assert a == b (same values)
    assert(a == b);

    // Step 5: Assert NOT (a != b)
    assert(!(a != b));

    // Step 6: Assert a != c (different seconds)
    assert(a != c);

    // Step 7: Assert NOT (a == c)
    assert(!(a == c));

    // Step 8: Print test completion
    std::cout << "  Equality/inequality OK\n";
}

// ======================= TEST 5: Invalid Input Handling =======================

// Test that fromString() rejects invalid formats and throws appropriate exceptions
static void testInvalidFromString()
{
    // Step 1: Print test section header
    std::cout << "\n=====================================================\n";
    std::cout << "--- TEST 5: fromString() invalid input & types    ---";
    std::cout << "\n=====================================================\n";

    // ===== Test 1: Bad format (hyphen instead of colon) =====
    // Step 2: Initialize flag for invalid format
    bool badFmt = false;

    // Step 3: Attempt to parse invalid format "7-05"
    try
    {
        (void)TimeFormatter::fromString("7-05");
    }
    // Step 4: Catch invalid_argument exception
    catch (const std::invalid_argument&)
    {
        badFmt = true;
    }

    // Step 5: Print result
    std::cout << "fromString(\"7-05\") throws invalid_argument? " << (badFmt ? "yes" : "no") << "\n";

    // Step 6: Assert exception was thrown
    assert(badFmt);

    // ===== Test 2: Non-numeric values =====
    // Step 7: Reset flag
    badFmt = false;

    // Step 8: Attempt to parse non-numeric "aa:bb"
    try
    {
        (void)TimeFormatter::fromString("aa:bb");
    }
    // Step 9: Catch invalid_argument exception
    catch (const std::invalid_argument&)
    {
        badFmt = true;
    }

    // Step 10: Print result
    std::cout << "fromString(\"aa:bb\") throws invalid_argument? " << (badFmt ? "yes" : "no") << "\n";

    // Step 11: Assert exception was thrown
    assert(badFmt);

    // ===== Test 3: Trailing colon (dangling separator) =====
    // Step 12: Reset flag
    badFmt = false;

    // Step 13: Attempt to parse malformed "07:05:"
    try
    {
        (void)TimeFormatter::fromString("07:05:");
    }
    // Step 14: Catch invalid_argument exception
    catch (const std::invalid_argument&)
    {
        badFmt = true;
    }

    // Step 15: Print result
    std::cout << "fromString(\"07:05:\") throws invalid_argument? " << (badFmt ? "yes" : "no") << "\n";

    // Step 16: Assert exception was thrown (parsing fails on dangling colon)
    assert(badFmt);

    // ===== Test 4: Numeric but out-of-range (hour too large) =====
    // Step 17: Initialize flag for out-of-range
    bool outRange = false;

    // Step 18: Attempt to parse valid format but invalid value "99:00"
    try
    {
        (void)TimeFormatter::fromString("99:00");
    }
    // Step 19: Catch out_of_range exception (thrown by ctor range check)
    catch (const std::out_of_range&)
    {
        outRange = true;
    }

    // Step 20: Print result
    std::cout << "fromString(\"99:00\") throws out_of_range? " << (outRange ? "yes" : "no") << "\n";

    // Step 21: Assert exception was thrown
    assert(outRange);

    // ===== Test 5: Negative hour =====
    // Step 22: Reset flag
    outRange = false;

    // Step 23: Attempt to parse negative value "-1:00"
    try
    {
        (void)TimeFormatter::fromString("-1:00");
    }
    // Step 24: Catch out_of_range exception
    catch (const std::out_of_range&)
    {
        outRange = true;
    }

    // Step 25: Print result
    std::cout << "fromString(\"-1:00\") throws out_of_range? " << (outRange ? "yes" : "no") << "\n";

    // Step 26: Assert exception was thrown
    assert(outRange);

    // Step 27: Print test completion
    std::cout << "  Invalid input paths OK\n";
}

// ======================= TEST 6: Boundary Values =======================

// Test that minimum and maximum valid times work correctly
static void testBoundaries()
{
    // Step 1: Print test section header
    std::cout << "\n===========================================\n";
    std::cout << "--- TEST 6: Boundary values round-trip ---";
    std::cout << "\n===========================================\n";

    // Step 2: Parse minimum valid time "00:00"
    Time a = TimeFormatter::fromString("00:00");

    // Step 3: Parse maximum valid time "23:59:59"
    Time b = TimeFormatter::fromString("23:59:59");

    // Step 4: Convert to string and print minimum time
    std::cout << "00:00 -> " << TimeFormatter::format(a) << " (expect 00:00:00)\n";

    // Step 5: Convert to string and print maximum time
    std::cout << "23:59:59 -> " << TimeFormatter::format(b) << " (expect 23:59:59)\n";

    // Step 6: Assert minimum boundary
    assert(a == Time(0,0,0));

    // Step 7: Assert maximum boundary
    assert(b == Time(23,59,59));

    // Step 8: Print test completion
    std::cout << "  Boundary values OK\n";
}

// ======================= TEST 7: Round-Trip Conversion =======================

// Test that format() -> fromString() preserves Time values
static void testRoundTripSample()
{
    // Step 1: Print test section header
    std::cout << "\n==================================================\n";
    std::cout << "--- TEST 7: Round-trip format/fromString()     ---";
    std::cout << "\n==================================================\n";

    // Step 2: Define array of representative Time samples to test
    // Includes: midnight, early morning, noon, afternoon, late night
    const Time samples[] =
    {
        Time(0,0,0),      // Midnight
        Time(7,5,0),      // Early morning
        Time(9,8,7),      // Single-digit handling
        Time(12,34,56),   // Afternoon
        Time(23,59,59)    // End of day
    };

    // Step 3: Get array size for loop
    const std::size_t numSamples = sizeof(samples) / sizeof(samples[0]);

    // Step 4: Loop through each sample time using classic for loop
    for (std::size_t i = 0; i < numSamples; ++i)
    {
        // Step 4a: Get current sample
        const Time& t = samples[i];

        // Step 4b: Convert Time to string using TimeFormatter
        std::string s = TimeFormatter::format(t);

        // Step 4c: Parse string back to Time using TimeFormatter
        Time r = TimeFormatter::fromString(s);

        // Step 4d: Print the round-trip result
        std::cout << "round-trip " << s << " -> " << TimeFormatter::format(r) << "\n";

        // Step 4e: Assert that parsed time equals original time
        assert(r == t);
    }

    // Step 5: Print test completion
    std::cout << "  Round-trip OK\n";
}

// ======================= MAIN TEST DRIVER =========================

// Main entry point - runs all Time unit tests
int main()
{
    // Step 1: Run Test 1 - constructors and format()
    testConstructionAndToString();

    // Step 2: Run Test 2 - fromString variants and formats
    testFromStringVariants();

    // Step 3: Run Test 3 - range validation in constructor and set()
    testRangeCheckingCtorAndSet();

    // Step 4: Run Test 4 - equality and inequality operators
    testEqualityAndInequality();

    // Step 5: Run Test 5 - invalid input handling and exception types
    testInvalidFromString();

    // Step 6: Run Test 6 - minimum and maximum boundary values
    testBoundaries();

    // Step 7: Run Test 7 - round-trip conversion consistency
    testRoundTripSample();

    // Step 8: Print final completion message
    std::cout << "\n===========================================\n";
    std::cout << "All Time tests PASSED!\n";
    std::cout << "===========================================\n";

    // Step 9: Return 0 (success) - all assertions passed
    return 0;
}
