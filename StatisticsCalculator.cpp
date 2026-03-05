#include "StatisticsCalculator.h"
#include <cmath>

// ======================= PEARSON CORRELATION COEFFICIENT =======================

// Calculate sample Pearson correlation coefficient between two data series
// Returns correlation r in range [-1.0, 1.0], or 0.0 for invalid inputs
double StatisticsCalculator::calculateSPCC(const double* xValues, const double* yValues, std::size_t n)
{
    // Step 1: Check if sufficient data (need at least 2 points for correlation)
    if (n < 2) return 0.0;

    // ===== PASS 1: Calculate means =====
    // Step 2: Sum all x values
    double sumX = 0.0;
    for (std::size_t i = 0; i < n; ++i)
    {
        sumX += xValues[i];
    }

    // Step 3: Sum all y values
    double sumY = 0.0;
    for (std::size_t i = 0; i < n; ++i)
    {
        sumY += yValues[i];
    }

    // Step 4: Calculate mean of x values
    double meanX = sumX / static_cast<double>(n);

    // Step 5: Calculate mean of y values
    double meanY = sumY / static_cast<double>(n);

    // ===== PASS 2: Calculate deviations and products =====
    // Step 6: Initialize accumulators for correlation formula
    double sumXY = 0.0;      // Σ[(xi - x̄)(yi - ȳ)] - numerator
    double sumX2 = 0.0;      // Σ(xi - x̄)² - x variance component
    double sumY2 = 0.0;      // Σ(yi - ȳ)² - y variance component

    // Step 7: Loop through all data points
    for (std::size_t i = 0; i < n; ++i)
    {
        // Step 7a: Calculate deviation of x from mean
        double dx = xValues[i] - meanX;

        // Step 7b: Calculate deviation of y from mean
        double dy = yValues[i] - meanY;

        // Step 7c: Accumulate product of deviations (numerator)
        sumXY += dx * dy;

        // Step 7d: Accumulate squared deviations for x
        sumX2 += dx * dx;

        // Step 7e: Accumulate squared deviations for y
        sumY2 += dy * dy;
    }

    // ===== STEP 3: Calculate correlation coefficient =====
    // Step 8: Calculate denominator (product of standard deviations)
    double denominator = std::sqrt(sumX2 * sumY2);

    // Step 9: Check for zero denominator (one or both variables have zero variance)
    // This occurs when all x values are identical or all y values are identical
    if (denominator < 1e-15)
    {
        // Step 9a: Cannot compute correlation - return 0
        // Example: All temperatures are 20°C (no variation)
        return 0.0;
    }

    // Step 10: Calculate final correlation coefficient
    // r = covariance / (std_dev_x × std_dev_y)
    // r = Σ[(xi - x̄)(yi - ȳ)] / sqrt[Σ(xi - x̄)² × Σ(yi - ȳ)²]
    double r = sumXY / denominator;

    // Step 11: Return correlation coefficient
    // Result is in range [-1.0, 1.0]:
    // - r = 1.0: Perfect positive correlation (y increases as x increases)
    // - r = 0.0: No linear relationship
    // - r = -1.0: Perfect negative correlation (y decreases as x increases)
    return r;
}
