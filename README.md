# EV Battery Lifecycle and Degradation Tracking System

A modular C application for maintaining EV battery records, recording periodic measurements, estimating State of Health (SOH), calculating capacity degradation, classifying battery condition and generating reports.

## Engineering domain
Electric vehicles, battery analytics and instrumentation.

## Problem statement
EV battery capacity changes with use and ageing. This application provides a simple record-based way to track measurements and analyse capacity trends. It uses simulated or historical readings; it does not connect to a physical battery.

## Features
- Register and update battery records (update API is available to the application).
- Record periodic battery measurements by cycle.
- Validate battery and measurement data.
- Calculate SOH, degradation and simplified energy estimates.
- Classify battery condition using configurable thresholds.
- Search batteries and generate text reports.
- Persist data in CSV files.

## Architecture
The application is divided into battery management, validation, analytics, storage and reporting modules. `main.c` controls the menu and calls the modules.

## Folder structure
- `src/`: C implementation
- `include/`: public C headers
- `tests/`: Google Test C++ test files
- `data/`: runtime CSV files
- `docs/`: proposal and design documentation
- `build/`: generated objects and executables

## C concepts
Functions, structures, enumerations, arrays, strings, pointers, constants, file handling, input validation and status codes.

## Prerequisites
- GCC
- G++
- GNU Make
- Google Test development library (`libgtest-dev` or equivalent)

On Debian/Ubuntu, install build tools and Google Test using your distribution's package manager.

## Build
```sh
make all
```

## Run
```sh
make run
```

## Test
```sh
make test
```

The test target links the C implementation with Google Test. C++ is used only for the test harness, as permitted by the assignment.

## Clean
```sh
make clean
```

## Sample calculation
Rated capacity = 100 Ah; measured capacity = 85 Ah.

SOH = (85 / 100) × 100 = 85%

Capacity degradation = 100 − 85 = 15%

## Sample data
The application creates `data/batteries.csv` and `data/measurements.csv` when records are saved. CSV files are runtime data and are not tracked by Git.

## Testing summary
The test suite covers battery insertion and searching, validation, SOH, degradation, energy estimation and health classification. It includes positive, negative and boundary tests.

## Error handling
Public functions return `StatusCode` values or validation results. File operations are checked. Invalid measurements and duplicate records are rejected.

## Student information
- Student: Tejas Tavadarkar
- Roll number: 612509057
- Division/Batch: N/A
- Branch: Instrumentation and Control Engineering
- College: COEP Technological University

## Known limitations
- No physical battery or BMS connection.
- Health estimates depend on supplied capacity measurements.
- Thresholds are project assumptions and must be configured for the selected battery chemistry and pack.
- CSV format is intentionally simple and does not support commas inside model names.

## Future enhancements
Hardware integration with an ESP32 or BMS, more detailed SOC estimation, battery chemistry-specific limits, trend visualisation and validated remaining-useful-life models.

## GitHub repository
Add the repository URL after publishing the project.
