# Project Proposal: EV Battery Lifecycle and Degradation Tracking System

## Student Information
- Student: Tejas Tavadarkar
- Roll number: 612509057
- Division/Batch: N/A
- Branch: Instrumentation and Control Engineering
- College: COEP Technological University

## Engineering Scenario
EV batteries gradually lose usable capacity due to ageing, charge-discharge cycles and operating conditions. Operators need a consistent way to maintain battery records and review performance history.

## Problem Statement
Without structured records, it is difficult to review battery measurements over time, identify capacity reduction and decide when inspection may be required. This project proposes a C-based application to store battery records, analyse periodic measurements and generate lifecycle reports.

## Proposed Solution
The application registers batteries, records voltage/current/temperature/capacity readings, calculates SOH and capacity degradation, classifies condition and generates reports using simulated or historical data.

## Target Users
EV maintenance technicians, fleet operators, battery management personnel, and engineering students/researchers.

## In Scope
Battery registration, measurement records, validation, SOH and degradation calculations, health classification, search, CSV storage and text reports.

## Out of Scope
Physical BMS integration, live sensor acquisition, charging control, cloud services, machine-learning prediction and certified safety functions.

## Functional Requirements
FR-01 register battery; FR-02 validate ID; FR-03 enter measurements; FR-04 validate measurements; FR-05 calculate SOH; FR-06 calculate degradation; FR-07 track cycles; FR-08 classify condition; FR-09 flag abnormal conditions; FR-10 search battery; FR-11 update battery; FR-12 compare records; FR-13 persist records; FR-14 generate reports.

## Non-Functional Requirements
Readable, modular, portable and maintainable C code; validated inputs; checked file operations; warning-free compilation; automated tests; Makefile automation; Git version control and complete documentation.

## Initial Modules
`main.c`, `battery_manager.c`, `validation.c`, `analytics.c`, `storage.c`, `report.c`.

## Planned C Concepts
Functions, structures, enums, arrays, strings, pointers, constants, file I/O and status codes.

## Expected Outcome
A modular C application that demonstrates engineering data management and basic battery lifecycle analytics. It is an educational analysis tool, not a certified battery diagnostic system.
