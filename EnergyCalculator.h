#ifndef ENERGY_CALCULATOR_H
#define ENERGY_CALCULATOR_H

/**
 * @class EnergyCalculator
 * @brief Energy conversion and validation utilities.
 * @details Provides static methods for converting solar radiation measurements
 *          to energy values with threshold-based validation.
 *
 * Responsibilities:
 * - Convert solar radiation (W/m²) to energy (kWh/m²)
 * - Apply noise threshold filtering (SR >= 100 W/m²)
 * - Handle 10-minute reading interval conversion
 */
class EnergyCalculator
{
public:
    /**
     * @brief Convert solar radiation to kWh/m² if above noise threshold.
     * @param sr_wm2 Solar radiation in W/m² (10-minute average)
     * @param out_kwh Converted energy in kWh/m² (output parameter)
     * @return true if SR >= 100 and conversion performed, false if below threshold
     *
     * @details Only processes readings with SR >= 100 W/m² (below is noise).
     * Conversion formula for 10-minute readings: kWh = SR_W / 6000
     * - SR in W/m² (power)
     * - 10 minutes = 1/6 hour
     * - 1000 W = 1 kW
     * - Therefore: kWh = (SR * 10/60) / 1000 = SR / 6000
     *
     * Example: SR = 600 W/m² -> 600/6000 = 0.1 kWh/m²
     */
    static bool srToKwhValid(float sr_wm2, double& out_kwh);
};

#endif // ENERGY_CALCULATOR_H
