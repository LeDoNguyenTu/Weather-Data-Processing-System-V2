#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include "DateTimeUtils.h"
#include "WeatherProcessor.h"

// ======================= INPUT VALIDATION HELPER =======================

// Prompt user for integer input within specified range
// Re-prompts on invalid input (non-integer or out of range)
static int askInt(const char* prompt, int lo, int hi)
{
    // Step 1: Loop until valid input received
    while (true)
    {
        // Step 2: Display prompt to user
        std::cout << prompt;

        // Step 3: Declare variable to hold user input
        int x;

        // Step 4: Attempt to read integer from input stream
        // Step 5: Check if read succeeded AND value in valid range [lo, hi]
        if (std::cin >> x && x >= lo && x <= hi)
        {
            // Step 5a: Valid input - return the value
            return x;
        }

        // Step 6: Input was invalid - clear error state
        // This allows next read attempt to work properly
        std::cin.clear();

        // Step 7: Discard invalid input from stream buffer up to newline
        // Prevents leftover characters from breaking next read
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Step 8: Print error message and loop back to re-prompt
        std::cout << "Invalid. Try again.\n";
    }
}

// ======================= MAIN APPLICATION =======================

// Interactive weather data analysis application
// Allows users to query loaded weather data with various analysis options
int main()
{
    // Step 1: Create WeatherProcessor instance to manage weather data
    WeatherProcessor proc;

    // Step 2: Open audit output file for logging loaded records
    std::ofstream audit("AuditRecord.csv");

    // Step 3: Check if file opened successfully
    if (!audit)
    {
        // Step 3a: If file open failed, print error
        std::cerr << "Cannot open AuditRecord.csv\n";

        // Step 3b: Exit program with failure code
        return 1;
    }

    // Step 4: Load all weather data from manifest file
    // Pass audit stream to log each loaded record
    if (!proc.loadFromDataSource("data/data_source.txt", &audit))
    {
        // Step 4a: If loading failed, print error
        std::cerr << "Failed to load data.\n";

        // Step 4b: Exit program with failure code
        return 1;
    }

    // Step 5: Print confirmation message about audit file
    std::cout << "Wrote audit to AuditRecord.csv (Date,Time,S[m/s],T[C],SR[W/m^2])\n";

    // Step 6: Set floating-point output format for all prints
    // Fixed notation (not scientific) with 1 decimal place
    std::cout << std::fixed << std::setprecision(1);

    // Step 7: Main application loop - continue until user exits
    while (true)
    {
        // Step 8: Display menu options
        std::cout << "\n=========================== Start ============================\n"
                  << "\nMenu Options:\n"
                  << "1. Average wind speed (km/h) and sample SD for a month/year\n"
                  << "2. Average ambient temperature and SD for EACH month of a year\n"
                  << "3. Sample Pearson Correlation Coefficient (sPCC) for a specified month\n"
                  << "4. Write yearly CSV (WindTempSolar.csv)\n"
                  << "5. Exit\n";

        // Step 9: Get user choice (1-5)
        int choice = askInt("\nChoice: ", 1, 5);

        // Step 10: Check if user selected exit option
        if (choice == 5) break;  // Exit main loop

        // ===== OPTION 1: Monthly Wind Statistics =====
        if (choice == 1)
        {
            // Step 11: Prompt for year (1-9999)
            int year  = askInt("Year: ", 1, 9999);

            // Step 12: Prompt for month (1-12)
            int month = askInt("Month (1-12): ", 1, 12);

            // Step 13: Compute wind statistics for requested month/year
            Stats w = proc.windStatsKmHFor(year, month);

            // Step 14: Check if data exists for this month
            if (w.N == 0)
            {
                // Step 14a: No data - print message
                std::cout << DateTimeUtils::monthName(month) << " " << year << ": No Data\n";
            }
            else
            {
                // Step 14b: Data exists - set precision and print results
                std::cout << std::fixed << std::setprecision(1);
                std::cout << DateTimeUtils::monthName(month) << " " << year << ":\n"
                          << "Average speed: " << w.mean << " km/h\n"
                          << "Sample stdev: " << w.sd << "\n";

                // Step 14c: Reset precision for next menu iteration
                std::cout << std::fixed << std::setprecision(2);
            }
        }
        // ===== OPTION 2: Annual Temperature Statistics =====
        else if (choice == 2)
        {
            // Step 15: Prompt for year
            int year = askInt("Year: ", 1, 9999);

            // Step 16: Set precision for output
            std::cout << std::fixed << std::setprecision(1);

            // Step 17: Print year header
            std::cout << year << "\n";

            // Step 18: Loop through all 12 months
            for (int m = 1; m <= 12; ++m)
            {
                // Step 18a: Compute temperature statistics for this month
                Stats t = proc.tempStatsCFor(year, m);

                // Step 18b: Check if data exists for this month
                if (t.N == 0)
                {
                    // Step 18b-i: No data - print "No Data"
                    std::cout << DateTimeUtils::monthName(m) << ": No Data\n";
                }
                else
                {
                    // Step 18b-ii: Data exists - print average and standard deviation
                    std::cout << DateTimeUtils::monthName(m)
                              << ": average: " << t.mean << " degrees C"
                              << ", stdev: " << t.sd << "\n";
                }
            }

            // Step 19: Reset precision for next menu iteration
            std::cout << std::fixed << std::setprecision(2);
        }
        // ===== OPTION 3: Pearson Correlation Coefficient =====
        else if (choice == 3)
        {
            // Step 20: Prompt for month (1-12)
            int month = askInt("Month (1-12): ", 1, 12);

            // Step 21: Declare variables for PCC results
            double s_t, s_r, t_r;                    // Correlation values
            bool has_s_t, has_s_r, has_t_r;          // Validity flags

            // Step 22: Calculate correlations for specified month (across all years)
            proc.calculateMonthlyPCC(month, s_t, s_r, t_r, has_s_t, has_s_r, has_t_r);

            // Step 23: Set precision for correlation output
            std::cout << std::fixed << std::setprecision(2);

            // Step 24: Print section header
            std::cout << "Sample Pearson Correlation Coefficient for "
                      << DateTimeUtils::monthName(month) << "\n";

            // Step 25: Print correlation between wind speed and temperature
            if (has_s_t)
            {
                // Step 25a: Data sufficient - print correlation value
                std::cout << "S_T: " << s_t << "\n";
            }
            else
            {
                // Step 25b: Data insufficient - print message
                std::cout << "S_T: insufficient data\n";
            }

            // Step 26: Print correlation between wind speed and solar radiation
            if (has_s_r)
            {
                // Step 26a: Data sufficient - print correlation value
                std::cout << "S_R: " << s_r << "\n";
            }
            else
            {
                // Step 26b: Data insufficient - print message
                std::cout << "S_R: insufficient data\n";
            }

            // Step 27: Print correlation between temperature and solar radiation
            if (has_t_r)
            {
                // Step 27a: Data sufficient - print correlation value
                std::cout << "T_R: " << t_r << "\n";
            }
            else
            {
                // Step 27b: Data insufficient - print message
                std::cout << "T_R: insufficient data\n";
            }
        }
        // ===== OPTION 4: Generate Annual Report CSV =====
        else if (choice == 4)
        {
            // Step 28: Prompt for year
            int year = askInt("Year: ", 1, 9999);

            // Step 29: Attempt to write annual report CSV
            if (proc.writeAnnualReportCSV("WindTempSolar.csv", year))
            {
                // Step 29a: Success - print confirmation
                std::cout << "Wrote to WindTempSolar.csv\n";
            }
            else
            {
                // Step 29b: Failure - print error message
                std::cout << "Failed to write WindTempSolar.csv\n";
            }
        }

        // Step 30: Print separator between menu iterations
        std::cout << "\n============================ End =============================\n";
    }

    // Step 31: Return success code on normal exit
    return 0;
}
