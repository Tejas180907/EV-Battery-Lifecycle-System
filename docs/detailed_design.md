# Detailed Software Design

## Architecture
Menu-driven interface in `main.c`; battery management, validation, analytics, storage and reporting are implemented in separate modules.

## Data structures
`Battery` stores ID, model, rated capacity, nominal/minimum/maximum voltage and maximum temperature. `BatteryMeasurement` stores battery ID, cycle number, voltage, current, temperature and measured capacity. `BatteryDatabase` contains fixed arrays of both records.

## Enumerations
`BatteryHealth`: HEALTHY, DEGRADED, CRITICAL. `StatusCode`: success, invalid input, not found, duplicate, storage full, file error and invalid data.

## Calculation rules
SOH (%) = measured capacity / rated capacity × 100.
Degradation (%) = 100 − SOH.
Simplified energy estimate (Wh) = voltage × discharged Ah.
Initial project classification: SOH >= 80% Healthy; 60% <= SOH < 80% Degraded; SOH < 60% Critical. These are configurable educational assumptions, not universal standards.

## Validation
IDs are non-empty and limited to alphanumeric characters, hyphen and underscore. Capacity and voltage must be finite and positive. Minimum voltage must be below maximum voltage; nominal voltage must lie within the configured range. Measurements must refer to a registered battery, use a positive cycle number, remain within configured voltage and temperature limits, and have positive capacity not exceeding rated capacity. For each battery, cycle numbers must increase.

## File storage
`data/batteries.csv` stores battery definitions. `data/measurements.csv` stores cycle measurements. Files are loaded at startup and written after successful additions and on exit. File errors return a status.

## Error handling
Functions return status codes or validation results. Null pointers, invalid arguments, duplicate IDs, missing records, capacity limits, malformed records and file failures are handled.

## Unit test plan
Google Test tests cover valid and invalid battery insertion, duplicate IDs, search, null database, battery validation, measurement validation, SOH, degradation, energy estimation and health classification boundaries.

## Build
`make all`, `make run`, `make test`, `make clean`.
