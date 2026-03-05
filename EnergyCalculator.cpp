#include "EnergyCalculator.h"

// ======================= SOLAR ENERGY CONVERSION =======================

// Convert solar radiation to kWh/m² if value meets threshold
// Only processes readings with SR >= 100 W/m² (below this is sensor noise)
bool EnergyCalculator::srToKwhValid(float sr_wm2, double& out_kwh)
{
    // Step 1: Check if solar radiation meets minimum threshold
    // Below 100 W/m² is considered sensor noise and ignored
    if (sr_wm2 < 100.0f)
    {
        // Step 1a: Below threshold - reject this reading
        return false;
    }

    // Step 2: Convert solar radiation to energy (kWh/m²)
    // Formula explanation:
    // - Input: SR in W/m² (watts per square meter) - power measurement
    // - Reading interval: 10 minutes (1/6 hour)
    // - Energy = Power × Time
    // - Energy (Wh) = SR (W/m²) × (10/60) hours = SR × (1/6)
    // - Energy (kWh) = Energy (Wh) / 1000 = SR × (1/6) / 1000 = SR / 6000
    //
    // Example calculation:
    // If SR = 600 W/m² for 10 minutes:
    // - Energy = 600 W/m² × (10/60) h = 100 Wh/m²
    // - Energy = 100 Wh/m² / 1000 = 0.1 kWh/m²
    // - Direct: 600 / 6000 = 0.1 kWh/m² ✓
    out_kwh = static_cast<double>(sr_wm2) / 6000.0;

    // Step 3: Conversion successful - return true
    return true;
}
