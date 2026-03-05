#include <iostream>
#include <string>
#include <cassert>
#include "Map.h"

// ======================= TESTING FRAMEWORK =======================

// Global counters for tracking test results
static int g_pass = 0, g_fail = 0;

// ======================= TEST HELPER FUNCTIONS =======================

// Generic boolean condition check
static void check(bool cond, const char* msg)
{
    // Step 1: Print test message with result (1=pass, 0=fail)
    std::cout << msg << " -> " << (cond ? 1 : 0) << "\n";

    // Step 2: Increment pass counter if condition is true
    if (cond) ++g_pass;
    // Step 3: Increment fail counter if condition is false
    else ++g_fail;
}

// Unsigned long long size equality check
static void check_eq_size(std::size_t actual, std::size_t expected, const char* msg)
{
    // Step 1: Compare actual size with expected size
    const bool ok = (actual == expected);

    // Step 2: Print test message with expected, actual, and result
    std::cout << msg << " | expected=" << expected << " actual=" << actual << " -> " << (ok?1:0) << "\n";

    // Step 3: Update pass/fail counter
    if (ok) ++g_pass;
    else ++g_fail;
}

// String equality check
static void check_eq_str(const std::string& actual, const std::string& expected, const char* msg)
{
    // Step 1: Compare actual string with expected string
    const bool ok = (actual == expected);

    // Step 2: Print test message with quoted strings and result
    std::cout << msg << " | expected=\"" << expected << "\" actual=\"" << actual << "\" -> " << (ok?1:0) << "\n";

    // Step 3: Update pass/fail counter
    if (ok) ++g_pass;
    else ++g_fail;
}

// Double equality check with epsilon tolerance
static void check_eq_double(double actual, double expected, const char* msg)
{
    // Step 1: Use small epsilon for floating-point comparison
    const double eps = 1e-9;

    // Step 2: Check if difference is within tolerance
    const bool ok = (std::abs(actual - expected) <= eps);

    // Step 3: Print test message with expected, actual, and result
    std::cout << msg << " | expected=" << expected << " actual=" << actual << " -> " << (ok?1:0) << "\n";

    // Step 4: Update pass/fail counter
    if (ok) ++g_pass;
    else ++g_fail;
}

// ======================= TEST STEPS =========================

// TEST 1: Basic map operations with integer keys and string values
static void testBasicOperationsIntString()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 1: Basic operations (int -> string) ------";
    std::cout << "\n====================================================\n";

    // Step 2: Create map to store student ID to name mapping
    Map<int, std::string> students;

    // Step 3: Insert first student record (key=12345, value="Alice")
    students.put(12345, "Alice");

    // Step 4: Insert second student record
    students.put(67890, "Bob");

    // Step 5: Insert third student record
    students.put(11111, "Charlie");

    // Step 6: Insert fourth student record
    students.put(22222, "Diana");

    // Step 7: Check that size is 4 after 4 insertions
    check_eq_size(students.size(), 4, "size after 4 puts");

    // Step 8: Retrieve and verify first student
    std::string val1 = students.get(12345);
    check_eq_str(val1, "Alice", "get(12345)");

    // Step 9: Retrieve and verify second student
    std::string val2 = students.get(67890);
    check_eq_str(val2, "Bob", "get(67890)");

    // Step 10: Retrieve and verify third student
    std::string val3 = students.get(11111);
    check_eq_str(val3, "Charlie", "get(11111)");
}

// TEST 2: Update existing key (overwrite previous value)
static void testUpdateExistingKey()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 2: Update existing key --------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create map and insert initial student record
    Map<int, std::string> students;
    students.put(12345, "Alice");

    // Step 3: Verify initial value
    check_eq_str(students.get(12345), "Alice", "initial put(12345, \"Alice\")");

    // Step 4: Check size is 1
    check_eq_size(students.size(), 1, "size before update");

    // Step 5: Update existing key with new value
    students.put(12345, "Alice Smith");

    // Step 6: Verify updated value
    check_eq_str(students.get(12345), "Alice Smith", "after put(12345, \"Alice Smith\")");

    // Step 7: Check size is still 1 (no new entry added)
    check_eq_size(students.size(), 1, "size after update (should not increase)");
}

// TEST 3: Contains operation (check if key exists)
static void testContainsKey()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 3: Contains key check --------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create map and insert some entries
    Map<int, std::string> students;
    students.put(12345, "Alice");
    students.put(67890, "Bob");
    students.put(11111, "Charlie");

    // Step 3: Check that existing keys return true
    check(students.contains(12345), "contains(12345) existing key");
    check(students.contains(67890), "contains(67890) existing key");
    check(students.contains(11111), "contains(11111) existing key");

    // Step 4: Check that non-existing keys return false
    check(!students.contains(99999), "!contains(99999) non-existing key");
    check(!students.contains(55555), "!contains(55555) non-existing key");
}

// TEST 4: Bracket operator [] for insertion and retrieval
static void testBracketOperator()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 4: Bracket operator [] -------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create map
    Map<int, std::string> students;

    // Step 3: Insert entry using bracket operator
    students[33333] = "Eve";

    // Step 4: Verify size increased to 1
    check_eq_size(students.size(), 1, "size after bracket insert");

    // Step 5: Retrieve using bracket operator
    std::string val = students[33333];
    check_eq_str(val, "Eve", "retrieve via bracket [33333]");

    // Step 6: Insert another entry using bracket operator
    students[44444] = "Frank";

    // Step 7: Verify size is now 2
    check_eq_size(students.size(), 2, "size after second bracket insert");

    // Step 8: Verify both entries exist
    check(students.contains(33333), "contains 33333 after bracket insert");
    check(students.contains(44444), "contains 44444 after bracket insert");
}

// TEST 5: Map with different value types (double for grades)
static void testMapWithDoubleValues()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 5: Map with double values (grades) ------";
    std::cout << "\n====================================================\n";

    // Step 2: Create map for student grades (int -> double)
    Map<int, double> grades;

    // Step 3: Insert grade records for multiple students
    grades.put(12345, 85.5);
    grades.put(67890, 92.0);
    grades.put(11111, 78.3);
    grades.put(22222, 88.7);
    grades.put(33333, 95.2);

    // Step 4: Verify size is 5
    check_eq_size(grades.size(), 5, "size after 5 grade insertions");

    // Step 5: Retrieve and verify first grade
    double g1 = grades.get(12345);
    check_eq_double(g1, 85.5, "get(12345) grade");

    // Step 6: Retrieve and verify second grade
    double g2 = grades.get(67890);
    check_eq_double(g2, 92.0, "get(67890) grade");

    // Step 7: Retrieve and verify last grade
    double g5 = grades.get(33333);
    check_eq_double(g5, 95.2, "get(33333) grade");
}

// TEST 6: Remove operation (delete entries)
static void testRemoveKey()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 6: Remove key operation ------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create map and insert entries
    Map<int, std::string> students;
    students.put(12345, "Alice");
    students.put(67890, "Bob");
    students.put(11111, "Charlie");
    students.put(22222, "Diana");

    // Step 3: Verify size is 4
    check_eq_size(students.size(), 4, "size before removal");

    // Step 4: Verify the key exists before removal
    check(students.contains(11111), "contains(11111) before removal");

    // Step 5: Remove an entry
    bool removed = students.remove(11111);

    // Step 6: Check that remove returned true
    check(removed, "remove(11111) returned true");

    // Step 7: Verify size decreased to 3
    check_eq_size(students.size(), 3, "size after removal");

    // Step 8: Verify the key no longer exists
    check(!students.contains(11111), "!contains(11111) after removal");

    // Step 9: Verify other keys still exist
    check(students.contains(12345), "contains(12345) still exists");
    check(students.contains(67890), "contains(67890) still exists");
    check(students.contains(22222), "contains(22222) still exists");

    // Step 10: Try to remove non-existing key
    bool removedFalse = students.remove(99999);

    // Step 11: Check that remove returned false
    check(!removedFalse, "remove(99999) returned false for non-existing");

    // Step 12: Verify size unchanged
    check_eq_size(students.size(), 3, "size unchanged after failed removal");
}

// TEST 7: Copy constructor (deep copy semantics)
static void testCopyConstructor()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 7: Copy constructor --------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create original map and insert entries
    Map<int, std::string> students;
    students.put(12345, "Alice");
    students.put(67890, "Bob");
    students.put(11111, "Charlie");
    students.put(22222, "Diana");

    // Step 3: Verify original size is 4
    check_eq_size(students.size(), 4, "original size");

    // Step 4: Create copy using copy constructor
    Map<int, std::string> studentsCopy = students;

    // Step 5: Verify copy has same size as original
    check_eq_size(studentsCopy.size(), 4, "copy size matches original");

    // Step 6: Verify copy contains same data
    check(studentsCopy.contains(12345), "copy contains(12345)");
    check_eq_str(studentsCopy.get(12345), "Alice", "copy.get(12345)");

    // Step 7: Verify copy contains other entries
    check(studentsCopy.contains(67890), "copy contains(67890)");
    check(studentsCopy.contains(11111), "copy contains(11111)");
    check(studentsCopy.contains(22222), "copy contains(22222)");

    // ===== Modify copy and verify independence =====
    // Step 8: Add new entry to copy
    studentsCopy.put(44444, "Frank");

    // Step 9: Verify copy size increased to 5
    check_eq_size(studentsCopy.size(), 5, "copy size after adding entry");

    // Step 10: Verify original size is still 4 (not affected by copy change)
    check_eq_size(students.size(), 4, "original size unchanged after copy modification");

    // Step 11: Verify original doesn't contain the new entry
    check(!students.contains(44444), "original !contains(44444) added to copy");

    // Step 12: Verify copy contains the new entry
    check(studentsCopy.contains(44444), "copy contains(44444) newly added");
}

// TEST 8: Clear operation (remove all entries)
static void testClear()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 8: Clear map operation -------------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create map and insert multiple entries
    Map<int, std::string> students;
    students.put(12345, "Alice");
    students.put(67890, "Bob");
    students.put(11111, "Charlie");
    students.put(22222, "Diana");

    // Step 3: Verify size is 4 before clear
    check_eq_size(students.size(), 4, "size before clear");
    check(!students.empty(), "!empty() before clear");

    // Step 4: Clear the map
    students.clear();

    // Step 5: Verify size is 0 after clear
    check_eq_size(students.size(), 0, "size after clear");

    // Step 6: Verify map is empty
    check(students.empty(), "empty() after clear");

    // Step 7: Verify no keys exist
    check(!students.contains(12345), "!contains(12345) after clear");
    check(!students.contains(67890), "!contains(67890) after clear");
    check(!students.contains(11111), "!contains(11111) after clear");
    check(!students.contains(22222), "!contains(22222) after clear");

    // Step 8: Verify new entries can be added after clear
    students.put(99999, "Eve");
    check_eq_size(students.size(), 1, "size after adding to cleared map");
    check_eq_str(students.get(99999), "Eve", "get(99999) from cleared map");
}

// TEST 9: Empty check operation
static void testEmptyCheck()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 9: Empty check operation -----------------";
    std::cout << "\n====================================================\n";

    // Step 2: Create empty map
    Map<int, std::string> map;

    // Step 3: Verify empty map returns true for empty()
    check(map.empty(), "empty() on newly created map");

    // Step 4: Verify size is 0
    check_eq_size(map.size(), 0, "size() on empty map");

    // Step 5: Add one entry
    map.put(123, "test");

    // Step 6: Verify empty() returns false
    check(!map.empty(), "!empty() after adding entry");

    // Step 7: Verify size is 1
    check_eq_size(map.size(), 1, "size() after adding entry");

    // Step 8: Clear the map
    map.clear();

    // Step 9: Verify empty() returns true again
    check(map.empty(), "empty() after clear");

    // Step 10: Verify size is 0 again
    check_eq_size(map.size(), 0, "size() after clear");
}

// TEST 10: Collision handling with multiple keys
static void testCollisionHandling()
{
    // Step 1: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 10: Collision handling (stress test) -----";
    std::cout << "\n====================================================\n";

    // Step 2: Create map to stress-test hash table
    Map<int, std::string> map;

    // Step 3: Insert multiple entries to trigger potential collisions
    // Using various key values that may hash to same bucket
    for (int i = 1; i <= 20; ++i)
    {
        map.put(i * 1000, "value_" + std::to_string(i));
    }

    // Step 4: Verify all 20 entries were inserted
    check_eq_size(map.size(), 20, "size after 20 insertions");

    // Step 5: Verify all entries can be retrieved correctly
    bool allFound = true;
    for (int i = 1; i <= 20; ++i)
    {
        int key = i * 1000;
        std::string expected = "value_" + std::to_string(i);
        if (map.get(key) != expected)
        {
            allFound = false;
            break;
        }
    }
    check(allFound, "all 20 entries retrieved correctly");

    // Step 6: Update some entries
    map.put(5000, "updated_value_5");
    check_eq_str(map.get(5000), "updated_value_5", "get(5000) after update");

    // Step 7: Verify size didn't increase
    check_eq_size(map.size(), 20, "size unchanged after update");

    // Step 8: Remove some entries
    map.remove(1000);
    map.remove(2000);
    map.remove(3000);

    // Step 9: Verify size decreased to 17
    check_eq_size(map.size(), 17, "size after 3 removals");

    // Step 10: Verify removed entries are gone
    check(!map.contains(1000), "!contains(1000) after removal");
    check(!map.contains(2000), "!contains(2000) after removal");
    check(!map.contains(3000), "!contains(3000) after removal");
}

// ======================= SUMMARY =========================

// Print test results and exit code
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

// Main entry point - orchestrates all map unit tests
int main()
{
    // Step 1: Print test suite header
    std::cout << "=== Map Test Program (Hash Table Implementation) ===\n\n";

    // Step 2: Run Test 1 - basic operations with int keys and string values
    testBasicOperationsIntString();

    // Step 3: Run Test 2 - update existing key
    testUpdateExistingKey();

    // Step 4: Run Test 3 - contains key check
    testContainsKey();

    // Step 5: Run Test 4 - bracket operator
    testBracketOperator();

    // Step 6: Run Test 5 - map with double values
    testMapWithDoubleValues();

    // Step 7: Run Test 6 - remove operation
    testRemoveKey();

    // Step 8: Run Test 7 - copy constructor
    testCopyConstructor();

    // Step 9: Run Test 8 - clear operation
    testClear();

    // Step 10: Run Test 9 - empty check
    testEmptyCheck();

    // Step 11: Run Test 10 - collision handling stress test
    testCollisionHandling();

    // Step 12: Print final results
    int code = 0;
    printTotalsAndExitCode(code);

    // Step 13: Return exit code (0=success, 1=failure)
    return code;
}
