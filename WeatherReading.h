#ifndef WEATHER_READING_H_INCLUDED
#define WEATHER_READING_H_INCLUDED

#include "Date.h"
#include "Time.h"

/**
 * @struct WeatherReading
 * @brief One parsed weather record with comparison operators for AVL tree.
 * @details Encapsulates weather data including date, time, wind speed,
 *          temperature, and solar radiation. Provides comparison operators
 *          to enable sorting by chronological order for AVL tree storage.
 */
struct WeatherReading
{
    Date  d;              ///< Date of the weather reading
    Time  t;              ///< Time of the weather reading
    float wind_ms;        ///< Wind speed in meters per second
    float temp_c;         ///< Temperature in Celsius
    float solar_wm2;      ///< Solar radiation in watts per square meter

    /**
     * @brief Default constructor.
     * @details Initializes all members with default values (date and time
     *          objects use their defaults, floats initialized to 0.0f).
     */
    WeatherReading();

    /**
     * @brief Parameterized constructor.
     * @param [in] d_      The date of the reading.
     * @param [in] t_      The time of the reading.
     * @param [in] s_ms    Wind speed in meters per second.
     * @param [in] t_C     Temperature in Celsius.
     * @param [in] sr_Wm2  Solar radiation in watts per square meter.
     */
    WeatherReading(const Date& d_, const Time& t_, float s_ms, float t_C, float sr_Wm2);

    /**
     * @brief Less-than comparison operator for AVL tree ordering.
     * @param [in] other The WeatherReading to compare with.
     * @return True if this reading occurs before the other reading
     *         (chronologically earlier), false otherwise.
     * @details Compares readings by date first, then by time (hour, minute, second).
     */
    bool operator<(const WeatherReading& other) const;

    /**
     * @brief Equality comparison operator.
     * @param [in] other The WeatherReading to compare with.
     * @return True if both readings have identical date and time, false otherwise.
     */
    bool operator==(const WeatherReading& other) const;
};

#endif // WEATHER_READING_H_INCLUDED
