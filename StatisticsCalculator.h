#ifndef STATISTICS_CALCULATOR_H
#define STATISTICS_CALCULATOR_H

#include <cstddef>

/**
 * @class StatisticsCalculator
 * @brief Statistical computation utilities.
 * @details Provides static methods for computing correlation coefficients
 *          and other statistical measures.
 *
 * Responsibilities:
 * - Calculate Pearson correlation coefficient
 * - Handle edge cases (n < 2, zero variance)
 * - Support paired data analysis
 */
class StatisticsCalculator
{
public:
    /**
     * @brief Calculate sample Pearson correlation coefficient (sPCC).
     * @param xValues Pointer to array of x values
     * @param yValues Pointer to array of y values
     * @param n Number of paired data points (must be >= 2 for meaningful result)
     * @return Correlation coefficient r in range [-1.0, 1.0], or 0.0 if n < 2
     *
     * @details Computes Pearson product-moment correlation using formula in Lab 9
     *
     * Interpretation:
     * - r = 1.0: Perfect positive correlation
     * - r = 0.0: No linear correlation
     * - r = -1.0: Perfect negative correlation
     *
     * Returns 0.0 if:
     * - n < 2 (insufficient data)
     * - Either variable has zero variance (constant values)
     */
    static double calculateSPCC(const double* xValues, const double* yValues, std::size_t n);
};

#endif // STATISTICS_CALCULATOR_H
