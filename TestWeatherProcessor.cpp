#include "WeatherProcessor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <sys/stat.h>  // For directory checking

// ======================= TINY HARNESS =======================
// Step 1: Define global counters for test results
static int g_pass = 0, g_fail = 0;

// Step 2: Helper function to check boolean conditions
// Prints test message and updates pass/fail counters
static void check(bool cond, const char* msg)
{
    // Step 2a: Print test result with message and pass/fail indicator
    std::cout << msg << " -> " << (cond ? 1 : 0) << "\n";

    // Step 2b: Update appropriate counter based on test result
    if (cond) ++g_pass;
    else ++g_fail;
}

// Step 3: Helper function to compare unsigned 64-bit integers
// Prints expected vs actual values and updates counters
static void check_eq_u64(unsigned long long actual, unsigned long long expected, const char* msg)
{
    // Step 3a: Compare actual and expected values
    const bool ok = (actual == expected);

    // Step 3b: Print detailed comparison result
    std::cout << msg << " | expected=" << expected << " actual=" << actual << " -> " << (ok ? 1 : 0) << "\n";

    // Step 3c: Update pass/fail counters
    if (ok) ++g_pass;
    else ++g_fail;
}

// Step 4: Helper function to compare strings
// Prints expected vs actual strings and updates counters
static void check_eq_str(const std::string& actual, const std::string& expected, const char* msg)
{
    // Step 4a: Compare actual and expected strings
    const bool ok = (actual == expected);

    // Step 4b: Print detailed string comparison result
    std::cout << msg << " | expected=\"" << expected << "\" actual=\"" << actual << "\" -> " << (ok ? 1 : 0) << "\n";

    // Step 4c: Update pass/fail counters
    if (ok) ++g_pass;
    else ++g_fail;
}

// Step 5: Helper function to compare floating-point values with tolerance
// Uses epsilon value to allow for floating-point precision differences
static void check_close(double actual, double expected, double eps, const char* msg)
{
    // Step 5a: Check if difference is within epsilon tolerance
    const bool ok = std::fabs(actual - expected) <= eps;

    // Step 5b: Print detailed floating-point comparison result
    std::cout << msg << " | expected~=" << expected << " actual=" << actual << " eps=" << eps << " -> " << (ok ? 1 : 0) << "\n";

    // Step 5c: Update pass/fail counters
    if (ok) ++g_pass;
    else ++g_fail;
}

// ========================= FILE HELPERS =========================

// Step 6: Helper function to check if directory exists
// Returns true if directory exists, false otherwise
static bool directoryExists(const char* path)
{
    // Step 6a: Use stat to check directory existence
    struct stat info;
    if (stat(path, &info) != 0)
        return false;  // Cannot access path
    else if (info.st_mode & S_IFDIR)
        return true;   // Directory exists
    else
        return false;  // Path exists but is not a directory
}

// Step 7: Helper function to write text content to a file
// Returns true on success, false on failure
static bool write_text_file(const std::string& path, const std::string& text)
{
    // Step 7a: Open output file stream
    std::ofstream out(path.c_str());

    // Step 7b: Check if file opened successfully
    if (!out) return false;

    // Step 7c: Write text content to file
    out << text;

    // Step 7d: Return success
    return true;
}

// ========================= TEST DATA BUILDERS =========================

// Step 8: Define constants for test file names
// NOTE: CSV must be in data/ directory because WeatherProcessor prepends "data/"
static const char* CSV_FILENAME = "TEST_unit_data.csv";              // Just filename
static const char* CSV_PATH     = "data/TEST_unit_data.csv";         // Full path in data/
static const char* DS_NAME      = "TEST_data_source.txt";            // Data source file
static const char* REPORT_OUT   = "TEST_WindTempSolar.csv";          // Test report output

// Step 9: Build a minimal CSV file with various test cases
// Includes valid data, invalid data (N/A), and multiple months
// Format: SR,S,T,WAST with various data patterns for comprehensive testing
static std::string buildTinyCSV()
{
    // Step 9a: Return CSV string with header and test data rows
    // Row 1: SR < 100 (below threshold), March data
    // Row 2: SR >= 100, March data (valid for solar calculation)
    // Row 3: SR >= 100, March data (valid)
    // Row 4: SR >= 100, April data (different month)
    // Row 5: Invalid wind speed (N/A) - should be skipped
    // Row 6: SR < 100, March data (below threshold)
    // Row 7: SR < 100, May data (below threshold)
    return
        "SR,S,T,WAST\n"
        "50,5,20,1/3/2025 10:00\n"
        "600,7,21,1/3/2025 10:10:00\n"
        "150,9,22,1/3/2025 10:20:00\n"
        "200,3,25,02-04-2025 12:00\n"
        "300,N/A,23,1/3/2025 11:00\n"
        "90,8,23.5,1/3/2025 11:10\n"
        "50,4,18,1-5-2025 09:00\n";
}

// Step 10: Setup test environment by creating test files
// Creates both CSV data file and data source manifest file
// IMPORTANT: Creates CSV in data/ directory where WeatherProcessor expects it
static bool setupTestFiles()
{
    // Step 10a: Print setup section header
    std::cout << "\n====================================================\n";
    std::cout << "--- SETUP: write test CSV & data_source ------------";
    std::cout << "\n====================================================\n";
    std::cout << "NOTE: Using TEST_ prefixed files to preserve real data\n";

    // Step 10b: Check if data/ directory exists
    if (!directoryExists("data"))
    {
        std::cout << "[WARNING] data/ directory does not exist!\n";
        std::cout << "[INFO] Creating data/ directory...\n";
        #ifdef _WIN32
            system("mkdir data");
        #else
            system("mkdir -p data");
        #endif
    }

    // Step 10c: Write test CSV file in data/ directory
    // WeatherProcessor prepends "data/" to filename, so CSV must be there
    bool okCSV = write_text_file(CSV_PATH, buildTinyCSV());

    // Step 10d: Write test data source file with just the filename
    // WeatherProcessor will prepend "data/" when reading this
    bool okDS  = write_text_file(DS_NAME, CSV_FILENAME);

    // Step 10e: Verify CSV file was written successfully
    check(okCSV, "write data/TEST_unit_data.csv");

    // Step 10f: Verify data source file was written successfully
    check(okDS,  "write TEST_data_source.txt");

    // Step 10g: Return success only if both files were created
    return okCSV && okDS;
}

// ========================= TEST STEPS =========================

// Step 11: Test loading data from data source file
// Loads CSV data through WeatherProcessor and captures audit log
static bool loadProcessor(WeatherProcessor& wp, std::ostringstream& audit)
{
    // Step 11a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 1: loadFromDataSource ---------------------";
    std::cout << "\n====================================================\n";

    // Step 11b: Attempt to load data from TEST data source file
    bool ok = wp.loadFromDataSource(DS_NAME, &audit);

    // Step 11c: Verify load operation succeeded
    check(ok, "loadFromDataSource(..) succeeded");

    // Step 11d: Return load result
    return ok;
}

// Step 12: Test that data was loaded by checking statistics
// Verifies that valid data exists by checking if March stats have data
static void testDataLoaded(const WeatherProcessor& wp)
{
    // Step 12a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 2: data successfully loaded ---------------";
    std::cout << "\n====================================================\n";

    // Step 12b: Get March wind statistics to verify data was loaded
    Stats wMar = wp.windStatsKmHFor(2025, 3);

    // Step 12c: Verify March has data (N should be greater than 0)
    check(wMar.N > 0, "March has wind data (N > 0)");

    // Step 12d: Verify we have expected 4 valid March records
    // (5 March rows in CSV - 1 with invalid wind speed N/A = 4 valid)
    check_eq_u64(wMar.N, 4ULL, "March has 4 valid wind records");
}


// Step 13: Test monthly statistics calculation for wind and temperature
// Validates mean, standard deviation, and MAD calculations
static void testMarchStats(const WeatherProcessor& wp)
{
    // Step 13a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 3: monthly stats (wind, temp) -------------";
    std::cout << "\n====================================================\n";

    // Step 13b: Calculate wind statistics for March 2025
    Stats wMar = wp.windStatsKmHFor(2025, 3);

    // Step 13c: Calculate temperature statistics for March 2025
    Stats tMar = wp.tempStatsCFor(2025, 3);

    // Step 13d: Verify wind March has 4 data points
    check_eq_u64(wMar.N, 4ULL, "wind March N=4");

    // Step 13e: Verify wind March mean is 26.1 km/h
    // Mean calculation: (5*3.6 + 7*3.6 + 9*3.6 + 8*3.6) / 4 = 26.1
    check_close(wMar.mean, 26.1, 1e-9, "wind March mean km/h");

    // Step 13f: Verify wind March sample standard deviation
    // SD calculation uses sample variance (divide by N-1)
    check_close(wMar.sd, std::sqrt(37.8), 1e-9, "wind March sample SD km/h");

    // Step 13g: Verify temperature March has 4 data points
    check_eq_u64(tMar.N, 4ULL, "temp March N=4");

    // Step 13h: Verify temperature March mean is 21.625캜
    // Mean calculation: (20 + 21 + 22 + 23.5) / 4 = 21.625
    check_close(tMar.mean, 21.625, 1e-12, "temp March mean C");

    // Step 13i: Verify temperature March sample standard deviation
    check_close(tMar.sd, 1.493036, 1e-5, "temp March sample SD C");
}

// Step 14: Test monthly solar energy calculation
// Validates SR threshold filtering (>= 100 W/m�) and energy conversion
static void testSolarByMonth(const WeatherProcessor& wp)
{
    // Step 14a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 4: monthly solar kWh/m^2 ------------------";
    std::cout << "\n====================================================\n";

    // Step 14b: Calculate solar energy for March 2025
    double srMar = wp.solarKWhFor(2025, 3);

    // Step 14c: Calculate solar energy for April 2025
    double srApr = wp.solarKWhFor(2025, 4);

    // Step 14d: Verify March solar energy (only SR >= 100 counted)
    // March has: 600 W/m� and 150 W/m� above threshold
    // Energy: (600 + 150) / 6000 = 0.125 kWh/m�
    check_close(srMar, 0.125, 1e-12, "solar March kWh/m^2 (SR>=100 only)");

    // Step 14e: Verify April solar energy
    // April has: 200 W/m� above threshold
    // Energy: 200 / 6000 = 0.0333... kWh/m�
    check_close(srApr, 200.0/6000.0, 1e-12, "solar April kWh/m^2");
}

// Step 15: Test statistics for multiple months
// Validates that different months are tracked independently
static void testMultipleMonths(const WeatherProcessor& wp)
{
    // Step 16a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 5: multiple months tracked ----------------";
    std::cout << "\n====================================================\n";

    // Step 15b: Get statistics for March, April, and May
    Stats wMar = wp.windStatsKmHFor(2025, 3);
    Stats wApr = wp.windStatsKmHFor(2025, 4);
    Stats wMay = wp.windStatsKmHFor(2025, 5);

    // Step 15c: Verify March has data
    check(wMar.N > 0, "March has wind data");

    // Step 15d: Verify April has data (1 record)
    check_eq_u64(wApr.N, 1ULL, "April has 1 wind record");

    // Step 15e: Verify May has data (1 record)
    check_eq_u64(wMay.N, 1ULL, "May has 1 wind record");

    // Step 15f: Verify April mean wind speed is correct
    // April: 3 m/s * 3.6 = 10.8 km/h
    check_close(wApr.mean, 10.8, 1e-9, "April wind mean 10.8 km/h");

    // Step 15g: Verify May mean wind speed is correct
    // May: 4 m/s * 3.6 = 14.4 km/h
    check_close(wMay.mean, 14.4, 1e-9, "May wind mean 14.4 km/h");
}

// Step 16: Test handling of empty months
// Validates that months with no data return zero statistics
static void testEmptyMonth(const WeatherProcessor& wp)
{
    // Step 16a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 7: empty month handling -------------------";
    std::cout << "\n====================================================\n";

    // Step 16b: Get statistics for January (no data in test file)
    Stats wJan = wp.windStatsKmHFor(2025, 1);
    Stats tJan = wp.tempStatsCFor(2025, 1);
    double srJan = wp.solarKWhFor(2025, 1);

    // Step 16c: Verify January wind has no data
    check_eq_u64(wJan.N, 0ULL, "January wind N=0 (no data)");

    // Step 16d: Verify January temperature has no data
    check_eq_u64(tJan.N, 0ULL, "January temp N=0 (no data)");

    // Step 16e: Verify January solar is zero
    check_close(srJan, 0.0, 1e-12, "January solar 0.0 (no data)");
}

// Step 17: Test annual report CSV file generation
// Validates file creation and content format
static void testReportFile(const WeatherProcessor& wp)
{
    // Step 17a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 7: writeAnnualReportCSV -------------------";
    std::cout << "\n====================================================\n";

    // Step 17b: Attempt to write annual report CSV to TEST file
    check(wp.writeAnnualReportCSV(REPORT_OUT, 2025), "writeAnnualReportCSV ok");

    // Step 17c: Open generated report file for reading
    std::ifstream rin(REPORT_OUT);

    // Step 17d: Verify file opened successfully
    check((bool)rin, "open written report");

    // Step 17e: Read and verify year header line
    std::string line;
    std::getline(rin, line);
    check_eq_str(line, "2025", "report year line");

    // Step 17f: Initialize flags for month presence checking
    bool sawMarch=false, sawApril=false, sawMay=false;

    // Step 17g: Scan report for expected month entries
    while (std::getline(rin, line))
    {
        // Step 17g-i: Skip empty lines
        if (line.empty()) continue;

        // Step 18g-ii: Check for March entry
        if (line.rfind("March,",0)==0)  sawMarch = true;

        // Step 17g-iii: Check for April entry
        if (line.rfind("April,",0)==0)  sawApril = true;

        // Step 17g-iv: Check for May entry
        if (line.rfind("May,",0)==0)    sawMay   = true;
    }

    // Step 17h: Verify March is present in report
    check(sawMarch, "report contains March");

    // Step 17i: Verify April is present in report
    check(sawApril, "report contains April");

    // Step 17j: Verify May is present in report
    check(sawMay,   "report contains May");
}

// Step 18: Test Pearson correlation coefficient calculation
// Validates calculateMonthlyPCC for wind-temp, wind-solar, temp-solar correlations
static void testPearsonCorrelation(const WeatherProcessor& wp)
{
    // Step 18a: Print test section header
    std::cout << "\n====================================================\n";
    std::cout << "--- TEST 8: Pearson correlation (PCC) --------------";
    std::cout << "\n====================================================\n";

    // Step 18b: Declare variables for correlation results
    double s_t, s_r, t_r;
    bool has_s_t, has_s_r, has_t_r;

    // Step 18c: Calculate March correlations
    wp.calculateMonthlyPCC(3, s_t, s_r, t_r, has_s_t, has_s_r, has_t_r);

    // Step 18d: Verify March has wind-temp correlation
    check(has_s_t, "March has wind-temp correlation");

    // Step 18e: Verify March has wind-solar correlation
    check(has_s_r, "March has wind-solar correlation");

    // Step 18f: Verify March has temp-solar correlation
    check(has_t_r, "March has temp-solar correlation");

    // Step 18g: Verify correlation values are in valid range [-1, 1]
    check(s_t >= -1.0 && s_t <= 1.0, "wind-temp correlation in range [-1,1]");
    check(s_r >= -1.0 && s_r <= 1.0, "wind-solar correlation in range [-1,1]");
    check(t_r >= -1.0 && t_r <= 1.0, "temp-solar correlation in range [-1,1]");
}

// Step 19: Print test summary and set exit code
// Displays total pass/fail counts and determines exit code
static void printTotalsAndExitCode(int& exitCodeOut)
{
    // Step 19a: Print total passed tests
    std::cout << "\nTOTAL PASSED: " << g_pass << "\n";

    // Step 19b: Print total failed tests
    std::cout << "TOTAL FAILED: " << g_fail << "\n";

    // Step 19c: Set exit code (0 if all passed, 1 if any failed)
    exitCodeOut = (g_fail == 0) ? 0 : 1;
}

// Step 20: Clean up temporary test files
// IMPORTANT: Only removes TEST_ prefixed files, preserves real data files
static void cleanupFiles()
{
    // Step 20a: Print cleanup section header
    std::cout << "\n====================================================\n";
    std::cout << "--- CLEANUP: removing TEST files only --------------";
    std::cout << "\n====================================================\n";

    // Step 20b: Remove test CSV file from data/ directory
    std::remove(CSV_PATH);
    std::cout << "Removed: " << CSV_PATH << "\n";

    // Step 20c: Remove test data source file
    std::remove(DS_NAME);
    std::cout << "Removed: " << DS_NAME << "\n";

    // Step 20d: Remove test report file
    std::remove(REPORT_OUT);
    std::cout << "Removed: " << REPORT_OUT << "\n";

}

// ========================= MAIN =========================

// Orchestrates all test execution and cleanup
int main()
{
    // Step 22a: Print test suite header
    std::cout << "====================================================\n";
    std::cout << "WeatherProcessor Unit Test (1=PASS, 0=FAIL)\n";
    std::cout << "====================================================\n";

    // Setup test environment
    if (!setupTestFiles())
    {
        // File setup failed - abort testing
        std::cout << "Failed to set up test files.\n";
        return 1;
    }

    // Create WeatherProcessor instance for testing
    WeatherProcessor wp;

    // Create audit log stream for capturing output
    std::ostringstream audit;

    // Load test data into processor
    if (!loadProcessor(wp, audit))
    {
        // Loading failed - print results and cleanup
        int code = 1;
        printTotalsAndExitCode(code);
        cleanupFiles();
        return code;
    }

    // ===== RUN ALL TEST CASES =====

    // Test data loading
    testDataLoaded(wp);

    // Test statistical calculations
    testMarchStats(wp);

    // Test solar energy calculations
    testSolarByMonth(wp);

    // Test multiple month tracking
    testMultipleMonths(wp);

    // Test empty month handling
    testEmptyMonth(wp);

    // Test report generation
    testReportFile(wp);

    // Test Pearson correlation coefficient
    testPearsonCorrelation(wp);

    // ===== FINALIZE TESTING =====

    // Print final test summary
    int code = 0;
    printTotalsAndExitCode(code);

    // Clean up test files (only TEST_ prefixed files)
    cleanupFiles();

    // Return exit code (0 = success, 1 = failure)
    return code;
}
