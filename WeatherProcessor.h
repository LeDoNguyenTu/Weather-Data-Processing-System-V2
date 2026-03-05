#ifndef WEATHER_PROCESSOR_H
#define WEATHER_PROCESSOR_H

#include <string>
#include <fstream>
#include <sstream>
#include "WeatherReading.h"
#include "AvlTree.h"
#include "Map.h"
#include "StringList.h"

// Forward declarations of utility classes (implementation in separate files)
class CSVParser;
class DateTimeParser;
class DateTimeUtils;
class EnergyCalculator;
class StatisticsCalculator;

/**
 * @struct Stats
 * @brief Container for statistical measures: count, mean, standard deviation, and mean absolute deviation.
 *
 * Used to report aggregated statistics for any month/year combination.
 */
struct Stats
{
    unsigned long long N{0};    ///< Number of data points in sample
    double mean{0.0};           ///< Arithmetic mean of values
    double sd{0.0};             ///< Sample standard deviation
    double mad{0.0};            ///< Mean absolute deviation from mean
};

/**
 * @class WeatherProcessor
 * @brief Load, store, and analyze weather station CSV data with efficient queries.
 *
 * @details WeatherProcessor coordinates weather data operations using utility classes
 * for parsing, filtering, and statistical analysis. It maintains two data structures:
 * - An AVL tree for sorted storage and efficient month/year queries
 * - A custom Map for O(1) deduplication of duplicate timestamps
 *
 * This class follows SOLID principles:
 * - Single Responsibility: Coordinates data operations (delegates parsing, calculations to utilities)
 * - Open/Closed: Extensible through utility classes
 * - Dependency Inversion: Depends on abstractions (utility interfaces)
 *
 * Design pattern: Facade - provides simplified interface to complex subsystem of utilities
 *
 * Core capabilities:
 * - Load CSV files with robust error handling and deduplication
 * - Retrieve wind speed, temperature, and solar radiation statistics for any month/year
 * - Calculate Pearson correlation coefficients between data fields
 * - Generate annual CSV reports for further analysis
 *
 * @see Map, AvlTree, WeatherReading, CSVParser, StatisticsCalculator
 */
class WeatherProcessor
{
public:
    /**
     * @brief Construct an empty processor ready to load data.
     */
    WeatherProcessor();

    /**
     * @brief Load data from multiple CSV files listed in a data source file.
     *
     * Reads a file containing paths to individual CSV files, loads each one,
     * and accumulates readings into the AVL tree with deduplication.
     *
     * @param [in] dataSourcePath Path to file containing CSV filenames (default: "data/data_source.txt")
     * @param [in,out] auditCsv Optional output stream; if provided, each accepted row is logged
     * @return true if at least one CSV file was successfully loaded, false if none loaded
     *
     * @see loadCSV()
     */
    bool loadFromDataSource(const std::string& dataSourcePath = "data/data_source.txt",
                            std::ostream* auditCsv = nullptr);

    /**
     * @brief Load data from a single CSV file, adding to existing data.
     *
     * Parses the CSV file using CSVParser utility, extracts date/time/values,
     * checks for duplicates using Map, and inserts valid unique records into AVL tree.
     * May be called multiple times to load different files into same processor.
     *
     * @param [in] csvPath Path to the CSV file to load
     * @param [in,out] auditCsv Optional output stream for audit logging
     * @return true if file opened and processed successfully, false otherwise
     *
     * @see loadFromDataSource(), CSVParser
     */
    bool loadCSV(const std::string& csvPath, std::ostream* auditCsv);

    /**
     * @brief Calculate wind speed statistics for a given month and year.
     *
     * Traverses the AVL tree to find all readings in the specified month/year,
     * computes mean wind speed (in km/h), sample standard deviation, and MAD.
     *
     * @param [in] year Calendar year (e.g., 2015)
     * @param [in] month Month number (1=January, 12=December)
     * @return Stats structure containing N, mean, sd, and mad for wind data
     *
     * @see Stats, tempStatsCFor()
     */
    Stats windStatsKmHFor(int year, int month) const;

    /**
     * @brief Calculate temperature statistics for a given month and year.
     *
     * Traverses the AVL tree to find all readings in the specified month/year,
     * computes mean temperature (in Celsius), sample standard deviation, and MAD.
     *
     * @param [in] year Calendar year
     * @param [in] month Month number (1–12)
     * @return Stats structure containing N, mean, sd, and mad for temperature data
     *
     * @see Stats, windStatsKmHFor()
     */
    Stats tempStatsCFor(int year, int month) const;

    /**
     * @brief Calculate total solar energy for a month across all years.
     *
     * Sums solar radiation energy only for readings where SR >= 100 W/m^2 (above noise threshold).
     * Uses EnergyCalculator utility for conversion to kWh/m².
     *
     * @param [in] year Calendar year
     * @param [in] month Month number (1–12)
     * @return Total solar energy in kWh/m^2 for the month, or 0.0 if no valid data
     *
     * @see EnergyCalculator
     */
    double solarKWhFor(int year, int month) const;

    /**
     * @brief Calculate Pearson correlation coefficients for a month across all years.
     *
     * Computes three pairwise correlations using StatisticsCalculator utility:
     * - S_T: Wind speed vs. Temperature
     * - S_R: Wind speed vs. Solar radiation (SR >= 100 only)
     * - T_R: Temperature vs. Solar radiation (SR >= 100 only)
     *
     * @param [in] month Month number (1–12)
     * @param [out] s_t_out Correlation coefficient: Wind vs Temperature, range [-1, 1]
     * @param [out] s_r_out Correlation coefficient: Wind vs Solar, range [-1, 1]
     * @param [out] t_r_out Correlation coefficient: Temp vs Solar, range [-1, 1]
     * @param [out] has_s_t true if S_T calculation is valid (sufficient data), false otherwise
     * @param [out] has_s_r true if S_R calculation is valid, false otherwise
     * @param [out] has_t_r true if T_R calculation is valid, false otherwise
     *
     * @see StatisticsCalculator::calculateSPCC()
     */
    void calculateMonthlyPCC(int month,
                             double& s_t_out, double& s_r_out, double& t_r_out,
                             bool& has_s_t, bool& has_s_r, bool& has_t_r) const;

    /**
     * @brief Write an annual summary report in CSV format.
     *
     * Generates a CSV file with one row per month, containing wind, temperature, and solar statistics
     * for all 12 months of the specified year. Uses DateTimeUtils for month names.
     *
     * @param [in] outPath Output filename (e.g., "WindTempSolar.csv")
     * @param [in] year Calendar year to summarize
     * @return true if write succeeded, false otherwise
     */
    bool writeAnnualReportCSV(const std::string& outPath, int year) const;

private:
    // =================== PRIMARY DATA STRUCTURES ===================
    AvlTree<WeatherReading> readings_;             ///< AVL tree storing all unique readings (sorted by timestamp)
    Map<unsigned long long, bool> seenTimestamps_; ///< Hash map for O(1) deduplication; tracks seen timestamps

    // =================== TRAVERSAL CONTEXT STRUCTURES ===================

    /**
     * @struct CountContext
     * @brief Scratch storage for counting/summing operations during AVL tree traversal.
     *
     * Used by callbacks to accumulate values across a filtered subset of readings.
     */
    struct CountContext
    {
        int year;
        int month;
        unsigned long long count; ///< Running count of matching records
        long double sum;          ///< Running sum of field values (wind, temp, etc.)
    };

    /**
     * @struct StatsContext
     * @brief Working values for computing standard deviation and MAD during traversal.
     *
     * Accumulates squared deviations and absolute deviations for statistical measures.
     */
    struct StatsContext
    {
        int year;
        int month;
        double mean;              ///< Mean value (used to compute deviations)
        long double sqSum;        ///< Sum of squared deviations from mean (for SD)
        long double absSum;       ///< Sum of absolute deviations from mean (for MAD)
        unsigned long long count; ///< Number of records in window
    };

    /**
     * @struct CollectContext
     * @brief Temporary array for collecting field values during traversal.
     *
     * Used to gather wind, temperature, or solar values into an array for analysis.
     */
    struct CollectContext
    {
        int month;
        double* array;            ///< Dynamically allocated array for values
        std::size_t capacity;     ///< Maximum array size
        std::size_t size;         ///< Current number of collected values
        bool solarFilter;         ///< If true, only collect if SR >= 100 W/m^2
    };

    /**
     * @struct PCCContext
     * @brief Paired data collector for Pearson correlation calculation.
     *
     * Collects three parallel arrays of wind speed, temperature, and solar values
     * so that Pearson correlations can be computed across all pairings.
     */
    struct PCCContext
    {
        int month;
        double* winds;            ///< Array of wind speed values (m/s)
        double* temps;            ///< Array of temperature values (degree C)
        double* solars;           ///< Array of solar radiation values (W/m^2)
        std::size_t count;        ///< Number of paired samples collected
        std::size_t capacity;     ///< Maximum array capacity
    };

    // Static contexts for AVL tree traversal callbacks
    static CountContext s_countCtx;
    static StatsContext s_statsCtx;
    static CollectContext s_collectCtx;
    static double s_solarSum;
    static bool s_hasSolar;
    static PCCContext s_pccCtx;

    // =================== PRIVATE HELPER METHODS ===================

    /**
     * @brief Internal method: append CSV data to AVL tree with deduplication.
     *
     * Parses CSV using CSVParser utility, validates data, checks for duplicate timestamps
     * using the Map, and inserts unique records into the AVL tree.
     *
     * @param [in] csvPath Path to CSV file
     * @param [in,out] audit Optional stream for logging each accepted row
     * @param [in,out] skippedByMonth Map tracking duplicate count per month (year*100+month as key)
     * @param [out] addedOut Count of rows successfully added to tree
     * @param [out] skippedOut Count of rows skipped due to duplicate timestamp
     * @return true if file parsed successfully, false if file open or processing failed
     */
    bool loadCSVAppend(const std::string& csvPath, std::ostream* audit,
                       Map<int, unsigned long long>& skippedByMonth,
                       unsigned long long& addedOut, unsigned long long& skippedOut);

    // =================== AVL TREE TRAVERSAL CALLBACKS ===================

    /**
     * @brief Callback: Count records matching target month/year.
     * Increments s_countCtx.count for each matching reading.
     */
    static void countMatchingMonth(const WeatherReading& r);

    /**
     * @brief Callback: Accumulate wind speed values for month/year.
     * Adds r.wind_ms to s_countCtx.sum for matching records.
     */
    static void sumMatchingMonth(const WeatherReading& r);

    /**
     * @brief Callback: Accumulate temperature values for month/year.
     * Adds r.temp_c to s_countCtx.sum for matching records.
     */
    static void sumTempMatchingMonth(const WeatherReading& r);

    /**
     * @brief Callback: Accumulate squared/absolute deviations for wind (SD and MAD).
     * Used after mean is known; updates s_statsCtx.sqSum and s_statsCtx.absSum.
     */
    static void computeDeviation(const WeatherReading& r);

    /**
     * @brief Callback: Accumulate squared/absolute deviations for temperature (SD and MAD).
     * Used after mean is known; updates s_statsCtx.sqSum and s_statsCtx.absSum.
     */
    static void computeTempDeviation(const WeatherReading& r);

    /**
     * @brief Callback: Sum solar energy for month/year (only SR >= 100).
     * Converts each valid reading to kWh using EnergyCalculator and adds to s_solarSum.
     */
    static void sumSolarEnergy(const WeatherReading& r);

    /**
     * @brief Callback: Check for presence of valid solar data (SR >= 100) in month/year.
     * Sets s_hasSolar to true if any qualifying record found.
     */
    static void checkSolarPresence(const WeatherReading& r);

    /**
     * @brief Callback: Collect paired wind/temperature/solar values for PCC calculation.
     * Appends values to s_pccCtx arrays if month and SR >= 100 conditions met.
     */
    static void collectDataForPCC(const WeatherReading& r);

    /**
     * @brief Callback: Count records for a given month across all years with SR filter.
     * Specialized version used in monthly PCC calculations (counts with SR >= 100).
     */
    static void countForMonthAllYears(const WeatherReading& r);
};

#endif // WEATHER_PROCESSOR_H
