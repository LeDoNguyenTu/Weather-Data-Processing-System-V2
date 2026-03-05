#include "WeatherProcessor.h"
#include "CSVParser.h"
#include "DateTimeParser.h"
#include "DateTimeUtils.h"
#include "EnergyCalculator.h"
#include "StatisticsCalculator.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

// =================== STATIC CONTEXT INITIALIZATION ===================

// Initialize static contexts for AVL tree traversal callbacks
WeatherProcessor::CountContext WeatherProcessor::s_countCtx;
WeatherProcessor::StatsContext WeatherProcessor::s_statsCtx;
WeatherProcessor::CollectContext WeatherProcessor::s_collectCtx;
double WeatherProcessor::s_solarSum = 0.0;
bool WeatherProcessor::s_hasSolar = false;
WeatherProcessor::PCCContext WeatherProcessor::s_pccCtx;

// =================== CONSTRUCTOR ===================

// Initialize empty processor ready to load data
WeatherProcessor::WeatherProcessor() : readings_(), seenTimestamps_() {}

// =================== DATA SOURCE LOADING ===================

// Load data from data source file listing multiple CSV files
// Reads manifest file, loads each CSV, accumulates results
bool WeatherProcessor::loadFromDataSource(const std::string& dataSourcePath,
                                          std::ostream* auditCsv)
{
    // Step 1: Print informational message about loading process
    std::cout << "[INFO] Loading from data source: " << dataSourcePath << "\n";

    // Step 2: Open data source file (contains list of CSV filenames)
    std::ifstream sourceFin(dataSourcePath.c_str());

    // Step 3: Check if file opened successfully
    if (!sourceFin)
    {
        // Step 3a: File open failed
        std::cerr << "[ERROR] Cannot open data source file: " << dataSourcePath << "\n";
        return false;
    }

    // Step 4: Initialize counters for overall loading statistics
    bool anyLoaded = false;           // Track if at least one CSV was loaded
    unsigned long long totalAdded = 0;   // Total records added across all files
    unsigned long long totalSkipped = 0; // Total duplicates skipped

    // Step 5: Map to track duplicates by month (key = year*100 + month)
    Map<int, unsigned long long> skippedByMonth;

    // ===== READ CSV FILENAMES FROM DATA SOURCE =====

    // Step 6: Read each line from data source file
    std::string csvFileName;
    while (std::getline(sourceFin, csvFileName))
    {
        // Step 6a: Trim whitespace from filename
        csvFileName = CSVParser::trim(csvFileName);

        // Step 6b: Skip empty lines and comments
        // Check BEFORE prepending path (check original filename)
        if (csvFileName.empty() || csvFileName[0] == '#')
        {
            // Step 6b-i: Skip empty lines and comments
            continue;
        }

        // Step 6c: Prepend "data/" directory to filename to create full path
        // Now filename is valid, so we can create the full path
        std::string fullPath = "data/" + csvFileName;

        // Step 6d: Print message about loading this CSV with full path
        std::cout << "[INFO] Loading CSV: " << csvFileName << "\n";

        // ===== LOAD CSV FILE =====
        // Step 7: Attempt to load this CSV file
        // Pass audit stream so each loaded row is logged
        unsigned long long added = 0, skipped = 0;
        if (loadCSVAppend(fullPath, auditCsv, skippedByMonth, added, skipped))
        {
            // Step 7a: CSV loaded successfully
            totalAdded += added;
            totalSkipped += skipped;
            anyLoaded = true;

            // Step 7b: Print summary for this file
            std::cout << "[INFO] Loaded: " << added << " rows, Skipped: " << skipped << " duplicates\n";
        }
        else
        {
            // Step 7c: CSV loading failed - print error with full path
            std::cerr << "[WARNING] Failed to load CSV: " << csvFileName << "\n";
        }
    }

    // Step 8: Close data source file
    sourceFin.close();

    // ===== PRINT SUMMARY STATISTICS =====
    // Step 9: Print overall summary
    std::cout << "[INFO] Total loaded: " << totalAdded << " records\n";
    std::cout << "[INFO] Total skipped: " << totalSkipped << " duplicates\n";

    // Step 10: Return success only if at least one CSV was successfully loaded
    return anyLoaded;
}

// =================== CSV LOADING ===================

// Load and append CSV file contents to AVL tree with deduplication
// Parses CSV, validates fields, detects duplicates, inserts into tree
bool WeatherProcessor::loadCSVAppend(const std::string& csvPath, std::ostream* audit,
                                     Map<int, unsigned long long>& skippedByMonth,
                                     unsigned long long& addedOut,
                                     unsigned long long& skippedOut)
{
    // Step 1: Print message indicating which file is being loaded
    std::cout << "[INFO] Loading CSV: " << csvPath << "\n";

    // Step 2: Initialize output counters to zero
    addedOut = 0;
    skippedOut = 0;

    // Step 3: Attempt to open CSV file for reading
    std::ifstream fin(csvPath.c_str());

    // Step 4: If file cannot be opened, return failure
    if (!fin)
    {
        std::cerr << "[ERROR] Cannot open CSV file: " << csvPath << "\n";
        return false;
    }

    // ===== READ HEADER ROW =====
    // Step 5: Read first line from CSV (header row)
    std::string headerLine;
    if (!std::getline(fin, headerLine))
    {
        // Step 5a: File is empty (no header)
        std::cerr << "[ERROR] CSV file is empty: " << csvPath << "\n";
        return false;
    }

    // Step 6: Parse header row into fields
    StringList headerFields;
    CSVParser::splitCSV(headerLine, headerFields);

    // Step 7: Find required columns using CSVParser utility
    int idxWAST = -1, idxS = -1, idxT = -1, idxSR = -1;
    if (!CSVParser::findColumns(headerFields, idxWAST, idxS, idxT, idxSR))
    {
        // Step 7a: Required columns not found in header
        std::cerr << "[ERROR] Required CSV columns not found. Need: WAST, S, T, SR\n";
        return false;
    }

    // ===== PROCESS DATA ROWS =====
    // Step 8: Initialize line counter for error reporting
    int lineNum = 1;  // Header is line 1

    // Step 9: Read each data row from CSV file
    std::string dataLine;
    while (std::getline(fin, dataLine))
    {
        // Step 9a: Increment line number for error messages
        ++lineNum;

        // Step 9b: Skip empty lines
        if (dataLine.empty()) continue;

        // Step 10: Parse data row into fields
        StringList fields;
        CSVParser::splitCSV(dataLine, fields);

        // Step 11: Validate that row has expected number of columns
        if (fields.size() < headerFields.size())
        {
            // Step 11a: Row has fewer columns than header
            std::cerr << "[WARNING] Line " << lineNum << ": Insufficient columns (skipping)\n";
            continue;
        }

        // ===== EXTRACT AND VALIDATE FIELDS =====
        // Step 12: Extract WAST field and parse date/time
        Date d;
        Time t;
        if (!DateTimeParser::parseWAST(fields[idxWAST], d, t))
        {
            // Step 12a: WAST parsing failed
            std::cerr << "[WARNING] Line " << lineNum << ": Invalid WAST field (skipping)\n";
            continue;
        }

        // Step 13: Parse wind speed field using CSVParser
        float windMs = 0.0f;
        if (!CSVParser::parseFloatField(fields[idxS], windMs))
        {
            // Step 13a: Wind speed parsing failed
            std::cerr << "[WARNING] Line " << lineNum << ": Invalid wind speed (skipping)\n";
            continue;
        }

        // Step 14: Parse temperature field using CSVParser
        float tempC = 0.0f;
        if (!CSVParser::parseFloatField(fields[idxT], tempC))
        {
            // Step 14a: Temperature parsing failed
            std::cerr << "[WARNING] Line " << lineNum << ": Invalid temperature (skipping)\n";
            continue;
        }

        // Step 15: Parse solar radiation field using CSVParser
        float solarWm2 = 0.0f;
        if (!CSVParser::parseFloatField(fields[idxSR], solarWm2))
        {
            // Step 15a: Solar radiation parsing failed
            std::cerr << "[WARNING] Line " << lineNum << ": Invalid solar radiation (skipping)\n";
            continue;
        }

        // ===== DEDUPLICATION CHECK =====
        // Step 16: Generate unique timestamp key using DateTimeUtils
        unsigned long long tsKey = DateTimeUtils::timestampKey(d, t);

        // Step 17: Check if timestamp already seen using Map for O(1) lookup
        if (seenTimestamps_.contains(tsKey))
        {
            // Step 17a: Duplicate timestamp found - skip this record
            ++skippedOut;

            // Step 17b: Update month-specific duplicate counter
            int monthCode = d.GetYear() * 100 + d.GetMonth();
            if (skippedByMonth.contains(monthCode))
            {
                skippedByMonth.put(monthCode, skippedByMonth.get(monthCode) + 1);
            }
            else
            {
                skippedByMonth.put(monthCode, 1);
            }
            continue;
        }

        // ===== INSERT INTO DATA STRUCTURES =====
        // Step 18: Mark timestamp as seen (add to deduplication map)
        seenTimestamps_.put(tsKey, true);

        // Step 19: Create WeatherReading object with parsed values
        WeatherReading reading(d, t, windMs, tempC, solarWm2);

        // Step 20: Store wind speed in original units (m/s)
        // Note: Function returns result in km/h after conversion during calculation
        reading.wind_ms = windMs;

        // Step 21: Insert reading into AVL tree (auto-sorted by timestamp)
        readings_.insert(reading);

        // Step 22: Increment counter of records added
        ++addedOut;

        // ===== AUDIT LOGGING =====
        // Step 23: If audit stream provided, log this record
        if (audit)
        {
            // Step 23a: Format and write audit record
            // Format: Date, Time, Wind(m/s), Temp(C), SolarRad(W/m²)
            *audit << d.GetDay() << "-" << d.GetMonth() << "-" << d.GetYear() << ","
                   << t.hour() << ":" << t.minute() << ":" << t.second() << ","
                   << windMs << "," << tempC << "," << solarWm2 << "\n";
        }
    }

    // Step 24: Close CSV file
    fin.close();

    // Step 25: Return success (file was processed, regardless of record count)
    return true;
}

// =================== WIND STATISTICS ===================

// Calculate wind speed statistics for a given month and year
// Returns mean, standard deviation, and MAD for wind speed in km/h
Stats WeatherProcessor::windStatsKmHFor(int year, int month) const
{
    // Step 1: Initialize result structure
    Stats result;

    // ===== PASS 1: COUNT RECORDS =====
    // Step 2: Initialize count context for tree traversal
    s_countCtx.year = year;
    s_countCtx.month = month;
    s_countCtx.count = 0;
    s_countCtx.sum = 0.0;

    // Step 3: Traverse tree to COUNT matching records
    readings_.inOrderApply(countMatchingMonth);

    // Step 4: Check if any records found for this month
    if (s_countCtx.count == 0)
    {
        // Step 4a: No data for this month - return empty stats
        result.N = 0;
        result.mean = 0.0;
        result.sd = 0.0;
        result.mad = 0.0;
        return result;
    }

    // ===== PASS 2: SUM WIND SPEEDS =====
    // Step 5: Reset sum for wind speed summation
    s_countCtx.sum = 0.0;  // Reset sum counter

    // Step 6: Traverse tree to SUM wind speeds for matching records
    readings_.inOrderApply(sumMatchingMonth);

    // Step 7: Calculate mean wind speed (in m/s)
    double meanWind = s_countCtx.sum / static_cast<double>(s_countCtx.count);

    // ===== PASS 3: CALCULATE DEVIATIONS =====
    // Step 8: Initialize stats context for deviation calculation
    s_statsCtx.year = year;
    s_statsCtx.month = month;
    s_statsCtx.mean = meanWind;
    s_statsCtx.sqSum = 0.0;
    s_statsCtx.absSum = 0.0;
    s_statsCtx.count = s_countCtx.count;

    // Step 9: Traverse tree with deviation callback
    readings_.inOrderApply(computeDeviation);

    // ===== CALCULATE FINAL STATISTICS =====
    // Step 10: Calculate sample standard deviation (in m/s)
    double variance = s_statsCtx.sqSum / static_cast<double>(s_statsCtx.count - 1);
    double sd = std::sqrt(variance);

    // Step 11: Calculate mean absolute deviation (in m/s)
    // MAD = Σ|xi - mean| / n
    double mad = s_statsCtx.absSum / static_cast<double>(s_statsCtx.count);

    // ===== CONVERT TO KM/H =====
    // Step 12: Store raw statistics (in m/s)
    result.N = s_countCtx.count;
    result.mean = meanWind;
    result.sd = sd;
    result.mad = mad;

    // Step 13: Convert from m/s to km/h before returning
    // Conversion factor: 1 m/s = 3.6 km/h
    result.mean = result.mean * 3.6;  // Convert mean to km/h
    result.sd = result.sd * 3.6;      // Convert SD to km/h
    result.mad = result.mad * 3.6;    // Convert MAD to km/h

    // Step 14: Return statistics (now in km/h)
    return result;
}

// =================== TEMPERATURE STATISTICS ===================

// Calculate temperature statistics for a given month and year
// Returns mean, standard deviation, and MAD for temperature
Stats WeatherProcessor::tempStatsCFor(int year, int month) const
{
    // Step 1: Initialize result structure
    Stats result;

    // ===== PASS 1: COUNT RECORDS AND SUM TEMPERATURES =====
    // Step 2: Initialize count context for tree traversal
    s_countCtx.year = year;
    s_countCtx.month = month;
    s_countCtx.count = 0;
    s_countCtx.sum = 0.0;

    // Step 3: Traverse tree with temperature summing callback
    readings_.inOrderApply(sumTempMatchingMonth);

    // Step 4: Check if any records found for this month
    if (s_countCtx.count == 0)
    {
        // Step 4a: No data for this month - return empty stats
        result.N = 0;
        result.mean = 0.0;
        result.sd = 0.0;
        result.mad = 0.0;
        return result;
    }

    // Step 5: Calculate mean temperature
    double meanTemp = s_countCtx.sum / static_cast<double>(s_countCtx.count);

    // ===== PASS 2: CALCULATE DEVIATIONS =====
    // Step 6: Initialize stats context for deviation calculation
    s_statsCtx.year = year;
    s_statsCtx.month = month;
    s_statsCtx.mean = meanTemp;
    s_statsCtx.sqSum = 0.0;
    s_statsCtx.absSum = 0.0;
    s_statsCtx.count = s_countCtx.count;

    // Step 7: Traverse tree with temperature deviation callback
    readings_.inOrderApply(computeTempDeviation);

    // ===== CALCULATE FINAL STATISTICS =====
    // Step 8: Calculate sample standard deviation
    double variance = s_statsCtx.sqSum / static_cast<double>(s_statsCtx.count - 1);
    double sd = std::sqrt(variance);

    // Step 9: Calculate mean absolute deviation
    double mad = s_statsCtx.absSum / static_cast<double>(s_statsCtx.count);

    // Step 10: Store results in output structure
    result.N = s_countCtx.count;
    result.mean = meanTemp;
    result.sd = sd;
    result.mad = mad;

    // Step 11: Return statistics
    return result;
}

// =================== SOLAR ENERGY CALCULATION ===================

// Calculate total solar energy for a month across all years
// Only processes readings where SR >= 100 W/m² (above noise threshold)
double WeatherProcessor::solarKWhFor(int year, int month) const
{
    // Step 1: Initialize context for solar energy summation
    s_solarSum = 0.0;
    s_hasSolar = false;
    s_collectCtx.month = month;
    s_countCtx.year = year;

    // Step 2: Traverse tree and accumulate solar energy for matching month
    // Uses EnergyCalculator utility for conversion
    readings_.inOrderApply(sumSolarEnergy);

    // Step 3: Check if any valid solar data found (SR >= 100)
    if (!s_hasSolar)
    {
        // Step 3a: No valid solar data for this month
        return 0.0;
    }

    // Step 4: Return accumulated solar energy in kWh/m²
    return s_solarSum;
}

// =================== PEARSON CORRELATION COEFFICIENT ===================

// Calculate Pearson correlation coefficients for a month across all years
// Computes three pairwise correlations: wind-temp, wind-solar, temp-solar
void WeatherProcessor::calculateMonthlyPCC(int month,
                                           double& s_t_out, double& s_r_out, double& t_r_out,
                                           bool& has_s_t, bool& has_s_r, bool& has_t_r) const
{
    // ===== PASS 1: COUNT VALID RECORDS (SR >= 100) =====
    // Step 1: Initialize count context for records with valid solar data
    s_countCtx.month = month;
    s_countCtx.count = 0;
    s_countCtx.sum = 0.0;

    // Step 2: Count records for this month with SR >= 100
    readings_.inOrderApply(countForMonthAllYears);

    // Step 3: Check if sufficient data for correlation
    if (s_countCtx.count < 2)
    {
        // Step 3a: Insufficient data - mark all as invalid
        s_t_out = 0.0;
        s_r_out = 0.0;
        t_r_out = 0.0;
        has_s_t = false;
        has_s_r = false;
        has_t_r = false;
        return;
    }

    // ===== PASS 2: ALLOCATE ARRAYS AND COLLECT DATA =====
    // Step 4: Allocate arrays for data collection
    double* winds = new double[s_countCtx.count];
    double* temps = new double[s_countCtx.count];
    double* solars = new double[s_countCtx.count];

    // Step 5: Initialize PCC context for data collection
    s_pccCtx.month = month;
    s_pccCtx.winds = winds;
    s_pccCtx.temps = temps;
    s_pccCtx.solars = solars;
    s_pccCtx.count = 0;
    s_pccCtx.capacity = s_countCtx.count;

    // Step 6: Traverse tree and collect paired wind/temp/solar values
    readings_.inOrderApply(collectDataForPCC);

    // Step 7: Get actual collected count (may be less than capacity due to filtering)
    std::size_t actualCount = s_pccCtx.count;

    // ===== PASS 3: CALCULATE CORRELATIONS =====
    // Step 8: Initialize output flags
    has_s_t = false;
    has_s_r = false;
    has_t_r = false;

    // Step 9: Calculate wind-temperature correlation (always valid if count >= 2)
    if (actualCount >= 2)
    {
        // Step 9a: Use StatisticsCalculator utility for sPCC calculation
        s_t_out = StatisticsCalculator::calculateSPCC(winds, temps, actualCount);
        has_s_t = true;
    }

    // Step 10: Calculate wind-solar correlation (only if solar variance > 0)
    if (actualCount >= 2)
    {
        // Step 10a: Check if solar values have variation
        double minSolar = solars[0], maxSolar = solars[0];
        for (std::size_t i = 1; i < actualCount; ++i)
        {
            if (solars[i] < minSolar) minSolar = solars[i];
            if (solars[i] > maxSolar) maxSolar = solars[i];
        }

        if (minSolar < maxSolar)
        {
            // Step 10b: Solar has variation - calculate correlation
            s_r_out = StatisticsCalculator::calculateSPCC(winds, solars, actualCount);
            has_s_r = true;
        }
    }

    // Step 11: Calculate temperature-solar correlation (only if both have variance)
    if (actualCount >= 2)
    {
        // Step 11a: Calculate correlation
        t_r_out = StatisticsCalculator::calculateSPCC(temps, solars, actualCount);
        has_t_r = true;
    }

    // ===== CLEANUP =====
    // Step 12: Free allocated arrays
    delete[] winds;
    delete[] temps;
    delete[] solars;
}

// =================== ANNUAL REPORT CSV ===================

// Write an annual summary report in CSV format
// One row per month, containing wind, temperature, and solar statistics
// Format includes Mean Absolute Deviation (MAD) alongside standard deviation
bool WeatherProcessor::writeAnnualReportCSV(const std::string& outPath, int year) const
{
    // Step 1: Open output file
    std::ofstream fout(outPath.c_str());
    if (!fout)
    {
        std::cerr << "[ERROR] Cannot open output file: " << outPath << "\n";
        return false;
    }

    // Step 2: DECLARE monthsWithData variable HERE
    int monthsWithData = 0;  // ← THIS LINE IS REQUIRED!

    // Step 3: Count months with data
    for (int m = 1; m <= 12; ++m)
    {
        Stats windStats = windStatsKmHFor(year, m);
        Stats tempStats = tempStatsCFor(year, m);
        double solarEnergy = solarKWhFor(year, m);

        // Count month if ANY field has data
        if (windStats.N > 0 || tempStats.N > 0 || solarEnergy > 0.0)
        {
            ++monthsWithData;
        }
    }

    // Step 4: Write year header
    fout << year << "\n";

    // Step 5: If entire year has no data, write "No Data" and exit
    if (monthsWithData == 0)
    {
        fout << "No Data\n";
        fout.close();
        return true;
    }

    // Step 6: Process each month - ONLY output if month has data
    for (int m = 1; m <= 12; ++m)
    {
        // Get statistics for this month
        Stats windStats = windStatsKmHFor(year, m);
        Stats tempStats = tempStatsCFor(year, m);
        double solarEnergy = solarKWhFor(year, m);

        // Skip entire month if ALL fields are empty
        if (windStats.N == 0 && tempStats.N == 0 && solarEnergy <= 1e-9)  // Fixed float comparison
        {
            continue;  // Don't output this month at all
        }

        // Month has at least some data - output the row
        fout << DateTimeUtils::monthName(m) << ",";

        // Output wind stats (or blank if missing)
        if (windStats.N > 0)
        {
            fout << std::fixed << std::setprecision(1)
                 << windStats.mean
                 << "(" << windStats.sd << ", " << windStats.mad << ")";
        }
        fout << ",";

        // Output temperature stats (or blank if missing)
        if (tempStats.N > 0)
        {
            fout << std::fixed << std::setprecision(1)
                 << tempStats.mean
                 << "(" << tempStats.sd << ", " << tempStats.mad << ")";
        }
        fout << ",";

        // Output solar energy (or blank if missing)
        if (solarEnergy > 1e-9)  // Fixed float comparison
        {
            fout << std::fixed << std::setprecision(1) << solarEnergy;
        }

        fout << "\n";
    }

    fout.close();
    return true;
}

// =================== AVL TREE TRAVERSAL CALLBACKS ===================

// Callback: Count records matching target month/year
// Increments s_countCtx.count for each matching reading
void WeatherProcessor::countMatchingMonth(const WeatherReading& r)
{
    // Step 1: Check if reading belongs to target month/year using DateTimeUtils
    if (DateTimeUtils::isMonthYear(r, s_countCtx.year, s_countCtx.month))
    {
        // Step 1a: Matching record found - increment count
        ++s_countCtx.count;
    }
}

// Callback: Accumulate wind speed values for month/year
// Adds r.wind_ms to s_countCtx.sum for matching records
void WeatherProcessor::sumMatchingMonth(const WeatherReading& r)
{
    // Step 1: Check if reading belongs to target month/year
    if (DateTimeUtils::isMonthYear(r, s_countCtx.year, s_countCtx.month))
    {
        // Step 1a: Matching record - add wind speed to sum
        s_countCtx.sum += r.wind_ms;
    }
}

// Callback: Accumulate temperature values for month/year
// Adds r.temp_c to s_countCtx.sum for matching records
void WeatherProcessor::sumTempMatchingMonth(const WeatherReading& r)
{
    // Step 1: Check if reading belongs to target month/year
    if (DateTimeUtils::isMonthYear(r, s_countCtx.year, s_countCtx.month))
    {
        // Step 1a: Matching record - add temperature to sum
        s_countCtx.sum += r.temp_c;

        // Step 1b: Also increment count
        ++s_countCtx.count;
    }
}

// Callback: Accumulate squared/absolute deviations for wind (SD and MAD)
// Used after mean is known; updates s_statsCtx.sqSum and s_statsCtx.absSum
void WeatherProcessor::computeDeviation(const WeatherReading& r)
{
    // Step 1: Check if reading belongs to target month/year
    if (DateTimeUtils::isMonthYear(r, s_statsCtx.year, s_statsCtx.month))
    {
        // Step 2: Calculate deviation from mean
        double deviation = r.wind_ms - s_statsCtx.mean;

        // Step 3: Add squared deviation (for standard deviation calculation)
        s_statsCtx.sqSum += deviation * deviation;

        // Step 4: Add absolute deviation (for MAD calculation)
        s_statsCtx.absSum += (deviation < 0 ? -deviation : deviation);
    }
}

// Callback: Accumulate squared/absolute deviations for temperature (SD and MAD)
// Used after mean is known; updates s_statsCtx.sqSum and s_statsCtx.absSum
void WeatherProcessor::computeTempDeviation(const WeatherReading& r)
{
    // Step 1: Check if reading belongs to target month/year
    if (DateTimeUtils::isMonthYear(r, s_statsCtx.year, s_statsCtx.month))
    {
        // Step 2: Calculate deviation from mean
        double deviation = r.temp_c - s_statsCtx.mean;

        // Step 3: Add squared deviation (for standard deviation calculation)
        s_statsCtx.sqSum += deviation * deviation;

        // Step 4: Add absolute deviation (for MAD calculation)
        s_statsCtx.absSum += (deviation < 0 ? -deviation : deviation);
    }
}

// Callback: Sum solar energy for month/year (only SR >= 100)
// Converts each valid reading to kWh using EnergyCalculator and adds to s_solarSum
void WeatherProcessor::sumSolarEnergy(const WeatherReading& r)
{
    // Step 1: Check if reading belongs to target month AND year (both filters)
    if (DateTimeUtils::isMonthYear(r, s_countCtx.year, s_collectCtx.month))
    {
        // Step 2: Try converting solar radiation to energy using EnergyCalculator
        double kwh = 0.0;
        if (EnergyCalculator::srToKwhValid(r.solar_wm2, kwh))
        {
            // Step 2a: Conversion successful (SR >= 100) - accumulate energy
            s_solarSum += kwh;

            // Step 2b: Mark that valid solar data was found
            s_hasSolar = true;
        }
    }
}

// Callback: Check for presence of valid solar data (SR >= 100) in month/year
// Sets s_hasSolar to true if any qualifying record found
void WeatherProcessor::checkSolarPresence(const WeatherReading& r)
{
    // Step 1: Check if reading belongs to target month (all years)
    if (r.d.GetMonth() == s_collectCtx.month)
    {
        // Step 2: Check if solar radiation meets threshold (>= 100 W/m²)
        if (r.solar_wm2 >= 100.0f)
        {
            // Step 2a: Valid solar data found
            s_hasSolar = true;
        }
    }
}

// Callback: Collect paired wind/temperature/solar values for PCC calculation
// Appends values to s_pccCtx arrays if month and SR >= 100 conditions met
void WeatherProcessor::collectDataForPCC(const WeatherReading& r)
{
    // Step 1: Check if reading's month matches target month
    if (r.d.GetMonth() == s_pccCtx.month)
    {
        // Step 2: Check if solar radiation meets threshold for PCC
        // Only include pairs where SR >= 100 W/m² (above noise)
        if (r.solar_wm2 >= 100.0f)
        {
            // Step 3: Check if arrays have capacity for this record
            if (s_pccCtx.count < s_pccCtx.capacity)
            {
                // Step 3a: Store wind speed in array
                s_pccCtx.winds[s_pccCtx.count] = r.wind_ms;

                // Step 3b: Store temperature in array
                s_pccCtx.temps[s_pccCtx.count] = r.temp_c;

                // Step 3c: Store solar radiation in array
                s_pccCtx.solars[s_pccCtx.count] = r.solar_wm2;

                // Step 3d: Increment count of collected records
                ++s_pccCtx.count;
            }
        }
    }
}

// Callback: Count records for a given month across all years with SR filter
// Specialized version used in monthly PCC calculations (counts with SR >= 100)
void WeatherProcessor::countForMonthAllYears(const WeatherReading& r)
{
    // Step 1: Check if reading's month matches target month (checking all years)
    if (r.d.GetMonth() == s_countCtx.month)
    {
        // Step 2: Check if solar radiation meets threshold
        // Only count pairs where SR >= 100 W/m² for correlation analysis
        if (r.solar_wm2 >= 100.0f)
        {
            // Step 2a: Increment count of valid records
            ++s_countCtx.count;
        }
    }
}
