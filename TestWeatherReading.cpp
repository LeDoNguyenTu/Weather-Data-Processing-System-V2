#include "WeatherReading.h"
#include "AvlTree.h"
#include <iostream>
#include <cmath>
#include <string>

// ======================= TESTING FRAMEWORK =======================

// Global counters for tracking test results
static int g_pass = 0;  // Count of passed assertions
static int g_fail = 0;  // Count of failed assertions

// ======================= TEST HELPER FUNCTIONS =======================

// Generic equality check for any type supporting == operator
// Prints result and updates global pass/fail counters
template <typename T>
void check_eq(const T& actual, const T& expected, const char* msg)
{
    // Step 1: Compare actual value with expected value using == operator
    const bool ok = (actual == expected);

    // Step 2: Print test message with expected, actual, and result (1=pass, 0=fail)
    std::cout << msg << " | expected=" << expected << " actual=" << actual << " -> " << (ok ? 1 : 0) << "\n";

    // Step 3: Increment global pass counter if test passed
    if (ok) ++g_pass;
    // Step 4: Increment global fail counter if test failed
    else ++g_fail;
}

// Floating-point comparison with epsilon tolerance
// Used for comparing double values that may have rounding errors
static void check_close(double actual, double expected, double eps, const char* msg)
{
    // Step 1: Calculate absolute difference between actual and expected
    // Step 2: Check if difference is within tolerance (eps)
    const bool ok = std::fabs(actual - expected) <= eps;

    // Step 3: Print test message with expected~=, actual, epsilon, and result
    std::cout << msg << " | expected~=" << expected << " actual=" << actual << " eps=" << eps << " -> " << (ok ? 1 : 0) << "\n";

    // Step 4: Increment global pass counter if test passed
    if (ok) ++g_pass;
    // Step 5: Increment global fail counter if test failed
    else ++g_fail;
}

// ======================= UNIT TESTS =======================

// TEST 1: Verify default constructor initializes all fields correctly
// Default behavior: Date = 1/1/1000, Time = 00:00:00, all floats = 0.0f
static void test_default_ctor()
{
    // Step 1: Print test section header for visibility
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 1: Default constructor --------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create WeatherReading with default constructor
    // This should initialize with Date(), Time(), and 0.0f for all floats
    WeatherReading r;

    // ===== Verify Date default values =====
    // Step 3: Check day component defaults to 1
    check_eq(r.d.GetDay(),   1,    "Date default day");

    // Step 4: Check month component defaults to 1
    check_eq(r.d.GetMonth(), 1,    "Date default month");

    // Step 5: Check year component defaults to 1000
    check_eq(r.d.GetYear(),  1000, "Date default year");

    // ===== Verify Time default values =====
    // Step 6: Check hour defaults to 0
    check_eq(r.t.hour(),   0, "Time default hour");

    // Step 7: Check minute defaults to 0
    check_eq(r.t.minute(), 0, "Time default minute");

    // Step 8: Check second defaults to 0
    check_eq(r.t.second(), 0, "Time default second");

    // ===== Verify float fields default to 0.0 =====
    // Step 9: Check wind_ms defaults to 0.0 (with zero tolerance)
    check_close(r.wind_ms,   0.0, 0, "wind_ms default = 0");

    // Step 10: Check temp_c defaults to 0.0
    check_close(r.temp_c,    0.0, 0, "temp_c  default = 0");

    // Step 11: Check solar_wm2 defaults to 0.0
    check_close(r.solar_wm2, 0.0, 0, "solar_wm2 default = 0");
}

// TEST 2: Verify parameterized constructor stores values correctly
// Tests that all constructor parameters are properly stored in member variables
static void test_value_ctor_exact()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 2: Value constructor ----------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create Date with specific values (day=2, month=3, year=2025)
    Date d(2, 3, 2025);

    // Step 3: Create Time with specific values (14:15:30)
    Time t(14, 15, 30);

    // Step 4: Create WeatherReading with all parameters specified
    WeatherReading r(d, t, 5.5f, 21.75f, 812.0f);

    // ===== Verify Date values stored correctly =====
    // Step 5: Check day was stored
    check_eq(r.d.GetDay(),   2,    "Date stored day");

    // Step 6: Check month was stored
    check_eq(r.d.GetMonth(), 3,    "Date stored month");

    // Step 7: Check year was stored
    check_eq(r.d.GetYear(),  2025, "Date stored year");

    // ===== Verify Time values stored correctly =====
    // Step 8: Check hour was stored
    check_eq(r.t.hour(),   14, "Time stored hour");

    // Step 9: Check minute was stored
    check_eq(r.t.minute(), 15, "Time stored minute");

    // Step 10: Check second was stored
    check_eq(r.t.second(), 30, "Time stored second");

    // ===== Verify float values stored correctly =====
    // Step 11: Check wind_ms was stored (tolerance: 1e-9)
    check_close(r.wind_ms,   5.5,    1e-9, "wind_ms stored");

    // Step 12: Check temp_c was stored
    check_close(r.temp_c,    21.75,  1e-9, "temp_c stored");

    // Step 13: Check solar_wm2 was stored
    check_close(r.solar_wm2, 812.0,  1e-9, "solar_wm2 stored");
}

// TEST 3: Verify copy constructor and assignment operator create independent copies
// Tests that modifications to copy don't affect original (deep copy semantics)
static void test_copy_and_assign_independence()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 3: Copy/assignment independence -----------";
    std::cout << "\n====================================================\n";

    // ===== Test Copy Constructor =====
    // Step 2: Create original WeatherReading with specific values
    WeatherReading a(Date(1,1,2020), Time(6,30,0), 3.0f, 18.0f, 250.0f);

    // Step 3: Create copy using copy constructor (b = a)
    WeatherReading b = a;

    // Step 4: Modify field in copy (change temp_c to 25.0)
    b.temp_c = 25.0f;

    // Step 5: Verify original 'a' still has original temp_c value (18.0)
    // This proves copy is independent and changes don't propagate
    check_close(a.temp_c, 18.0, 1e-9, "copy: original unaffected by change to copy");

    // Step 6: Verify copy 'b' has the new temp_c value (25.0)
    check_close(b.temp_c, 25.0, 1e-9, "copy: changed value present in copy");

    // ===== Test Copy Assignment Operator =====
    // Step 7: Create default WeatherReading
    WeatherReading c;

    // Step 8: Use assignment operator to copy 'a' into 'c' (c = a)
    c = a;

    // Step 9: Modify field in assigned copy (change wind_ms to 9.0)
    c.wind_ms = 9.0f;

    // Step 10: Verify original 'a' still has original wind_ms value (3.0)
    // This proves assignment creates independent copy
    check_close(a.wind_ms, 3.0, 1e-9, "assign: original unaffected by change to assignee");

    // Step 11: Verify assigned copy 'c' has the new wind_ms value (9.0)
    check_close(c.wind_ms, 9.0, 1e-9, "assign: changed value present in assignee");
}

// TEST 4: Verify WeatherReading comparison operators work correctly for AVL tree
// Tests less-than and equality operators used for tree insertion and searching
static void test_comparison_operators()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 4: Comparison operators for AVL tree ------";
    std::cout << "\n====================================================\n";

    // ===== Test Less-Than Operator =====
    // Step 2: Create first reading at 2025-03-01 09:00:00
    WeatherReading r1(Date(1,3,2025), Time(9,0,0),  6.0f, 22.0f, 650.0f);

    // Step 3: Create second reading at 2025-03-01 10:00:00 (later same day)
    WeatherReading r2(Date(1,3,2025), Time(10,0,0), 7.0f, 23.0f, 720.0f);

    // Step 4: Create third reading at 2025-03-02 09:00:00 (next day)
    WeatherReading r3(Date(2,3,2025), Time(9,0,0),  8.0f, 24.0f, 800.0f);

    // Step 5: Verify r1 < r2 (earlier time on same day)
    check_eq(r1 < r2, true, "r1 < r2: earlier time on same day");

    // Step 6: Verify not (r2 < r1)
    check_eq(r2 < r1, false, "r2 < r1: not true (later time not less)");

    // Step 7: Verify r1 < r3 (earlier day)
    check_eq(r1 < r3, true, "r1 < r3: earlier day");

    // Step 8: Verify r3 > r1 (same as not r3 < r1)
    check_eq(r3 < r1, false, "r3 < r1: not true (later day not less)");

    // ===== Test Equality Operator =====
    // Step 9: Create reading with exact same date/time as r1
    WeatherReading r1_copy(Date(1,3,2025), Time(9,0,0),  99.0f, 99.0f, 99.0f);
    // Note: Different float values, but same date/time

    // Step 10: Verify r1 == r1_copy (equality based on date/time only)
    check_eq(r1 == r1_copy, true, "r1 == r1_copy: same date/time (ignores floats)");

    // Step 11: Verify r1 != r2 (different time)
    check_eq(r1 == r2, false, "r1 != r2: different time");

    // Step 12: Verify r1 != r3 (different date)
    check_eq(r1 == r3, false, "r1 != r3: different date");
}

// TEST 5: Verify AVL tree can insert and search WeatherReading objects
// Tests that WeatherReading works correctly with AVL tree operations
static void test_avl_tree_integration()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 5: AVL tree integration -------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create AVL tree to store WeatherReading objects
    AvlTree<WeatherReading> tree;

    // Step 3: Verify tree starts empty
    check_eq(tree.empty(), true, "tree starts empty");
    check_eq(tree.size(), (std::size_t)0, "tree size = 0");

    // ===== Test Insertions =====
    // Step 4: Create and insert first reading (2025-03-01 09:00:00)
    WeatherReading r1(Date(1,3,2025), Time(9,0,0),   6.0f, 22.0f, 650.0f);
    tree.insert(r1);

    // Step 5: Verify tree has 1 element
    check_eq(tree.size(), (std::size_t)1, "tree size = 1 after first insert");

    // Step 6: Create and insert second reading (2025-03-01 10:00:00)
    WeatherReading r2(Date(1,3,2025), Time(10,0,0), 7.0f, 23.0f, 720.0f);
    tree.insert(r2);

    // Step 7: Verify tree has 2 elements
    check_eq(tree.size(), (std::size_t)2, "tree size = 2 after second insert");

    // Step 8: Create and insert third reading (2025-03-02 09:00:00)
    WeatherReading r3(Date(2,3,2025), Time(9,0,0),  8.0f, 24.0f, 800.0f);
    tree.insert(r3);

    // Step 9: Verify tree has 3 elements
    check_eq(tree.size(), (std::size_t)3, "tree size = 3 after third insert");

    // ===== Test Searching =====
    // Step 10: Search for r1 (should be found)
    check_eq(tree.search(r1), true, "tree.search(r1) found");

    // Step 11: Search for r2 (should be found)
    check_eq(tree.search(r2), true, "tree.search(r2) found");

    // Step 12: Search for r3 (should be found)
    check_eq(tree.search(r3), true, "tree.search(r3) found");

    // ===== Test Duplicate Rejection =====
    // Step 13: Create reading with same date/time as r1 but different floats
    WeatherReading r1_dup(Date(1,3,2025), Time(9,0,0), 99.0f, 99.0f, 99.0f);

    // Step 14: Try to insert duplicate (should be rejected, size unchanged)
    tree.insert(r1_dup);
    check_eq(tree.size(), (std::size_t)3, "tree size unchanged after duplicate insert");

    // ===== Test Chronological Ordering =====
    // Step 15: Create reading with earlier date/time than all current readings
    WeatherReading r0(Date(1,1,2025), Time(0,0,0),  1.0f, 10.0f, 100.0f);

    // Step 16: Insert reading with earliest timestamp
    tree.insert(r0);

    // Step 17: Verify tree now has 4 elements
    check_eq(tree.size(), (std::size_t)4, "tree size = 4 after inserting earliest reading");

    // Step 18: Search for earliest reading (should be found despite inserting last)
    check_eq(tree.search(r0), true, "tree.search(r0) found (AVL maintains chronological order)");
}

// TEST 6: Verify edge cases - negative temperature, zero wind/solar
// Tests that constructor accepts and stores extreme but valid values
static void test_edge_values()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 6: Edge values (neg temp; zero SR) --------";
    std::cout << "\n====================================================\n";

    // Step 2: Create WeatherReading with edge case values
    // Last day of year at last second, zero wind, negative temperature, zero solar
    WeatherReading r(Date(31,12,2024), Time(23,59,59), 0.0f, -5.5f, 0.0f);

    // Step 3: Verify zero wind_ms can be stored
    check_close(r.wind_ms,   0.0, 1e-9, "wind_ms = 0 stored");

    // Step 4: Verify negative temperature can be stored
    check_close(r.temp_c,   -5.5, 1e-9, "negative temp stored");

    // Step 5: Verify zero solar_wm2 can be stored
    check_close(r.solar_wm2, 0.0, 1e-9, "solar_wm2 = 0 stored");
}


// Main entry point - runs all tests and reports summary
int main()
{
    // Step 1: Print test suite header
    std::cout << "WeatherReading Unit Test (1=PASS, 0=FAIL)\n";

    // Step 2: Run test 1 - default constructor
    test_default_ctor();

    // Step 3: Run test 2 - value constructor
    test_value_ctor_exact();

    // Step 4: Run test 3 - copy/assignment independence
    test_copy_and_assign_independence();

    // Step 5: Run test 4 - comparison operators for AVL tree
    test_comparison_operators();

    // Step 6: Run test 5 - AVL tree integration
    test_avl_tree_integration();

    // Step 7: Run test 6 - edge values
    test_edge_values();

    // Step 8: Print summary - total passed tests
    std::cout << "\nTOTAL PASSED: " << g_pass << "\n";

    // Step 9: Print summary - total failed tests
    std::cout << "TOTAL FAILED: " << g_fail << "\n";

    // Step 10: Return exit code: 0 if all tests passed, 1 if any failed
    return (g_fail == 0) ? 0 : 1;
}
