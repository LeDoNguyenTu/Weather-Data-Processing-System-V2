# Advanced Weather Analytics System
**ICT283 Data Structures & Abstraction — Murdoch University (2025)**

An extension of Assignment 1, this C++11 application ingests meteorological data from
**multiple CSV files**, applies advanced statistical analysis, and reports monthly trends
via a CLI menu. Key additions include an AVL Tree with function pointers, a custom
Map class, Pearson Correlation Coefficient computation, and Mean Absolute Deviation output.

---

## Features

| Menu Option | Description | Output |
|---|---|---|
| 1 | Average wind speed + std dev for a given month/year | Screen |
| 2 | Average temperature + std dev for each month of a year | Screen |
| 3 | Sample Pearson Correlation Coefficient (S_T, S_R, T_R) for a given month across all years | Screen |
| 4 | Combined wind, temperature & solar report with stdev and MAD | `WindTempSolar.csv` |

---

## Technical Highlights

- **Custom AVL Tree (`AvlTree.h`)** — self-balancing BST template with function pointer support
  for flexible, reusable in-order/pre-order/post-order traversal callbacks; O(log n) insert & lookup
- **Custom `Map<K,V>` class (`Map.h`)** — templated associative container built on top of the
  AVL Tree, used for keyed monthly data aggregation (bonus requirement, fully justified with pros/cons)
- **Sample Pearson Correlation Coefficient** — `StatisticsCalculator` computes sPCC across
  wind speed, air temperature and solar radiation pairings, fully decoupled from weather domain types
- **Mean Absolute Deviation (MAD)** — added to all average outputs alongside standard deviation
  in the `WindTempSolar.csv` report: `Month, avg(stdev, mad), avg(stdev, mad), solar`
- **Multi-file data loading** — reads all file names from `data/data_source.txt` and ingests
  multiple years of CSV data in a single program run
- **Dedicated utility classes** — `CSVParser`, `EnergyCalculator`, `DateTimeParser`,
  `DateTimeUtils`, `TimeFormatter` each handle one responsibility, minimising coupling across 15+ files
- **6 test suites** — `TestAvlTree`, `TestMap`, `TestDate`, `TestTime`,
  `TestWeatherProcessor`, `TestWeatherReading`; all expected values verified against spreadsheet calculations

---

## Project Structure

```
├── main.cpp                    # Entry point & menu loop
├── WeatherProcessor.h/.cpp     # Core analytics engine
├── WeatherReading.h/.cpp       # Data model for a single sensor reading
├── AvlTree.h                   # Custom self-balancing AVL Tree (template + function pointers)
├── Map.h                       # Custom Map class built on AVL Tree
├── StatisticsCalculator.h/.cpp # Pearson CC, std dev, MAD — decoupled from weather types
├── EnergyCalculator.h/.cpp     # Solar radiation unit conversion (W/m² → kWh/m²)
├── CSVParser.h/.cpp            # Dynamic column-detection CSV parser
├── Date.h/.cpp                 # Date class
├── Time.h/.cpp                 # Time class
├── DateFormatter.h/.cpp        # Date parsing and formatting
├── DateTimeParser.h/.cpp       # Combined datetime parsing
├── DateTimeUtils.h/.cpp        # Datetime utility helpers
├── TimeFormatter.h/.cpp        # Time formatting
├── StringList.h/.cpp           # Custom string list
├── TestAvlTree.cpp             # Unit tests — AVL Tree
├── TestMap.cpp                 # Unit tests — Map
├── TestDate.cpp                # Unit tests — Date
├── TestTime.cpp                # Unit tests — Time
├── TestWeatherProcessor.cpp    # Integration tests — analytics engine
├── TestWeatherReading.cpp      # Unit tests — WeatherReading
└── WindTempSolar.csv           # Sample output file
```

---

## How to Build & Run

1. Open `ICT283_LAB10_LeDo.cbp` (Lab 10) or `ICT283_LAB11_LeDo.cbp` (Lab 11) in **CodeBlocks**
2. Set C++ standard to **C++11**
3. Edit `data/data_source.txt` — list one data file name per line for multi-file loading
4. Click **Build → Run**

> The program reads all data file names from `data/data_source.txt` — no recompilation needed to add or switch datasets.

---

## Data Structures Used

| Structure | Type | Usage |
|---|---|---|
| `AvlTree<T>` | Custom template | Primary sorted storage with function pointer traversal |
| `Map<K,V>` | Custom template (AVL-backed) | Monthly keyed aggregation (bonus) |
| `std::map` | STL | Secondary keyed lookups where justified |
| `Vector<T>` | Custom template (from Assm 1) | Linear storage, wraps all array-like data |

---

## Data Source

Sensor data from [Murdoch University Weather Station](http://wwwmet.murdoch.edu.au/).
Columns used: `WAST` (datetime), `S` (wind speed m/s → km/h), `SR` (solar radiation W/m² → kWh/m²), `T` (air temperature °C).

---

## Technologies

`C++11` · `CodeBlocks` · `Doxygen` · `AVL Tree` · `Function Pointers` · `STL map` · `OOP` · `Statistical Analysis`
