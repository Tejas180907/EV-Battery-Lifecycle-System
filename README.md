
# EV Battery Lifecycle and Degradation Tracking System

A modular C-based application for monitoring Electric Vehicle (EV) battery health, tracking capacity degradation, analysing historical measurements and predicting Remaining Useful Life (RUL) using linear regression.

**Developed using:** C, GCC, GNU Make and Google Test  
**Domain:** EV Battery Analytics | Instrumentation | Data Analysis | Predictive Maintenance

---

## 1. Project Overview

EV batteries gradually lose their capacity due to ageing and repeated charge-discharge cycles. Monitoring this degradation is important for understanding battery condition and estimating its remaining useful life.

This project provides a menu-driven software system that stores battery specifications, records periodic measurements, calculates battery health parameters and estimates the battery's End-of-Life (EOL) cycle using historical degradation trends.

The application uses manually entered or simulated measurements. It does not currently communicate with a physical battery or Battery Management System (BMS).

## 2. Problem Statement

Battery capacity degradation makes it difficult to determine the present health and expected remaining life of an EV battery.

This project addresses the problem by providing a structured system for:
- Maintaining battery records.
- Recording periodic battery measurements.
- Calculating State of Health (SOH).
- Analysing capacity degradation.
- Classifying battery condition.
- Estimating Remaining Useful Life using historical SOH trends.
- Generating battery analysis reports.

## 3. Key Features

### Battery Management
- Register EV battery specifications.
- Maintain battery records using unique IDs.
- Search and retrieve battery information.

### Measurement Tracking
- Record cycle number.
- Store voltage, discharge current and temperature.
- Record measured battery capacity.
- Maintain measurement history using CSV files.

### Battery Health Analytics
- Calculate State of Health (SOH).
- Calculate capacity degradation percentage.
- Estimate simplified energy in Wh.
- Classify battery condition as Healthy, Degraded or Critical.

### Remaining Useful Life (RUL) Prediction
- Uses historical battery capacity measurements.
- Converts measured capacity into SOH.
- Implements least-squares linear regression in pure C.
- Calculates SOH degradation rate per cycle.
- Estimates the End-of-Life cycle.
- Calculates estimated remaining cycles.
- Reports the regression coefficient of determination (R²).

### Reporting and Storage
- Generate battery-specific text reports.
- Store battery records and measurements in CSV files.
- Load previously stored records when the application starts.

## 4. RUL Prediction Methodology

The prediction module uses linear regression to model the relationship between battery cycle number and State of Health.

### Step 1: Calculate SOH

SOH (%) = (Measured Capacity / Rated Capacity) × 100

### Step 2: Fit the regression model

SOH = a + b × Cycle Number

Where:
- a = Regression intercept
- b = Regression slope
- Cycle Number = Historical charge-discharge cycle count

### Step 3: Estimate End-of-Life cycle

EOL Cycle = (EOL Threshold - a) / b

### Step 4: Calculate Remaining Useful Life

RUL (cycles) = EOL Cycle - Latest Recorded Cycle

The default EOL threshold is 80% SOH. It is a configurable project assumption, not a universal battery end-of-life standard.

The model requires at least three valid measurements with distinct cycle numbers and a decreasing SOH trend.

R² is reported to describe how well the fitted regression line represents the historical data.

**Important limitations:**
- This is a statistical linear-regression model, not a trained AI or machine-learning model.
- Predictions depend on the quality and quantity of historical measurements.
- Linear extrapolation may not represent actual future battery behaviour.
- RUL is estimated in cycles, not days.
- The model is intended for educational demonstration and is not a validated battery-life or safety assessment.

## 5. Sample Calculation

For a battery with:

- Rated capacity = 116 Ah
- Measured capacity = 101 Ah

SOH = (101 / 116) × 100 = 87.07%

Degradation = 100 - 87.07 = 12.93%

### Illustrative RUL dataset

| Cycle | Capacity (Ah) | SOH (%) |
|---:|---:|---:|
| 100 | 110 | 94.83 |
| 150 | 107 | 92.24 |
| 200 | 104 | 89.66 |
| 250 | 101 | 87.07 |

For this illustrative dataset, using an 80% EOL threshold:

- Estimated EOL cycle: approximately 386.67
- Latest recorded cycle: 250
- Estimated remaining cycles: approximately 136.67
- R²: approximately 1.000

These are simulated demonstration values, not actual measured or manufacturer-certified battery performance data.

## 6. Application Menu

| Option | Function |
|---|---|
| 1 | Register battery |
| 2 | Add battery measurement |
| 3 | View battery analysis |
| 4 | List batteries and summary |
| 5 | Generate battery report |
| 6 | Predict Remaining Useful Life (RUL) |
| 0 | Save and exit |

## 7. Project Architecture

The application follows a modular architecture.

| Module | Responsibility |
|---|---|
| main.c | Menu-driven application and user interaction |
| battery.c | Battery database initialization and common utilities |
| battery_manager.c | Battery registration, searching and measurement management |
| validation.c | Input and measurement validation |
| analytics.c | SOH, degradation, energy and health calculations |
| prediction.c | Linear regression and RUL estimation |
| storage.c | CSV-based data persistence |
| report.c | Battery reports and summaries |

## 8. Folder Structure

```text
EV-Battery-Lifecycle-System/
│
├── include/
│   ├── battery.h
│   ├── battery_manager.h
│   ├── validation.h
│   ├── analytics.h
│   ├── prediction.h
│   ├── storage.h
│   └── report.h
│
├── src/
│   ├── main.c
│   ├── battery.c
│   ├── battery_manager.c
│   ├── validation.c
│   ├── analytics.c
│   ├── prediction.c
│   ├── storage.c
│   └── report.c
│
├── tests/
│   ├── test_battery.cpp
│   ├── test_validation.cpp
│   ├── test_analytics.cpp
│   └── test_prediction.cpp
│
├── data/
│   └── Runtime CSV files
│
├── docs/
│   ├── project_proposal.md
│   ├── detailed_design.md
│   └── architecture.mmd
│
├── Makefile
├── README.md
├── .gitignore
└── LICENSE
```

## 9. Technologies and Concepts Used

### Programming and Tools
- C (C11)
- C++ (Google Test test harness)
- GCC and G++
- GNU Make
- Git and GitHub

### C Programming Concepts
- Functions and modular programming
- Structures and enumerations
- Arrays and pointers
- Strings and input validation
- File handling
- Header files and multi-file compilation
- Error handling using status codes

### Engineering and Analytical Concepts
- Battery State of Health
- Capacity degradation analysis
- Linear regression
- Least-squares estimation
- End-of-Life estimation
- Remaining Useful Life prediction

## 10. Build and Execution

### Prerequisites

- GCC
- G++
- GNU Make
- Google Test library

### Build the application

```bash
make all
```

### Run the application

```bash
make run
```

### Execute automated tests

```bash
make test
```

### Clean generated build files

```bash
make clean
```

The clean command removes generated build files and the latest report. It preserves saved battery CSV records.

## 11. Testing

The project uses Google Test with a C++ test harness to test the C modules.

**Latest verified test result: 29/29 tests passed.**

The test suite covers:
- Battery registration and searching.
- Input and measurement validation.
- SOH and degradation calculations.
- Energy estimation.
- Battery health classification.
- Linear regression-based RUL prediction.
- Insufficient historical data.
- Invalid inputs and duplicate cycle numbers.
- Missing battery records.

Compiler warning flags:

```bash
-Wall -Wextra -Wpedantic
```

## 12. Data Storage

The application maintains runtime data using:

- `data/batteries.csv`
- `data/measurements.csv`

Battery measurements include battery ID, cycle number, voltage, current, temperature and measured capacity.

CSV files are runtime data and are not required to be committed to the repository.

## 13. Current Limitations

- No physical battery or BMS integration.
- Measurements are manually entered or simulated.
- RUL estimation uses a simplified linear degradation assumption.
- Battery chemistry-specific degradation behaviour is not modelled.
- Voltage, current and temperature are recorded but are not inputs to the current RUL regression model.
- The default EOL threshold is a project assumption and should be configured for the intended battery application.

## 14. Future Enhancements

- ESP32 or BMS-based hardware integration.
- Real-time battery parameter acquisition.
- Battery chemistry-specific degradation modelling.
- Temperature and operating-condition-aware prediction.
- Historical degradation visualisation.
- More advanced and experimentally validated RUL prediction methods.
- Interactive battery monitoring dashboard.

## 15. Student Information

- **Student:** Tejas Tavadarkar
- **Roll Number:** 612509057
- **Branch:** Instrumentation and Control Engineering
- **College:** COEP Technological University

## 16. GitHub Repository

https://github.com/Tejas180907/EV-Battery-Lifecycle-System
