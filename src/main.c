
#include "battery.h"
#include "battery_manager.h"
#include "validation.h"
#include "analytics.h"
#include "prediction.h"
#include "storage.h"
#include "report.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BATTERY_FILE "data/batteries.csv"
#define MEASUREMENT_FILE "data/measurements.csv"
#define REPORT_FILE "data/latest_report.txt"
#define INPUT_SIZE 128

static int read_line(const char *prompt, char *buffer, size_t size)
{
    int ch;
    size_t length;

    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    length = strlen(buffer);

    if (length > 0U && buffer[length - 1U] == '\n') {
        buffer[length - 1U] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Discard remaining input. */
        }
    }

    return 1;
}

static int read_float_value(const char *prompt, float *value)
{
    char input[INPUT_SIZE];
    char *end = NULL;
    float parsed;

    if (!read_line(prompt, input, sizeof(input)) || input[0] == '\0') {
        return 0;
    }

    parsed = strtof(input, &end);

    if (end == input || *end != '\0') {
        return 0;
    }

    *value = parsed;
    return 1;
}

static int read_unsigned_value(const char *prompt, unsigned int *value)
{
    char input[INPUT_SIZE];
    char *end = NULL;
    unsigned long parsed;

    if (!read_line(prompt, input, sizeof(input)) ||
        input[0] == '\0' || input[0] == '-') {
        return 0;
    }

    parsed = strtoul(input, &end, 10);

    if (end == input || *end != '\0' ||
        parsed == 0UL || parsed > 4294967295UL) {
        return 0;
    }

    *value = (unsigned int)parsed;
    return 1;
}

static void add_battery_menu(BatteryDatabase *database)
{
    Battery battery;
    StatusCode status;

    memset(&battery, 0, sizeof(battery));

    if (!read_line("Battery ID: ", battery.battery_id,
                   sizeof(battery.battery_id)) ||
        !read_line("Model name: ", battery.model_name,
                   sizeof(battery.model_name)) ||
        !read_float_value("Rated capacity (Ah): ",
                          &battery.rated_capacity_ah) ||
        !read_float_value("Nominal voltage (V): ",
                          &battery.nominal_voltage) ||
        !read_float_value("Minimum voltage (V): ",
                          &battery.min_voltage) ||
        !read_float_value("Maximum voltage (V): ",
                          &battery.max_voltage) ||
        !read_float_value("Maximum temperature (C): ",
                          &battery.max_temperature_c)) {
        printf("Invalid or incomplete input.\n");
        return;
    }

    status = battery_add(database, &battery);
    printf("%s\n", status_code_message(status));

    if (status == STATUS_SUCCESS) {
        status = storage_save(database, BATTERY_FILE, MEASUREMENT_FILE);
        printf("Save: %s\n", status_code_message(status));
    }
}

static void add_measurement_menu(BatteryDatabase *database)
{
    BatteryMeasurement measurement;
    StatusCode status;

    memset(&measurement, 0, sizeof(measurement));

    if (!read_line("Battery ID: ", measurement.battery_id,
                   sizeof(measurement.battery_id)) ||
        !read_unsigned_value("Cycle number: ",
                             &measurement.cycle_number) ||
        !read_float_value("Voltage (V): ", &measurement.voltage) ||
        !read_float_value("Discharge current (A): ",
                          &measurement.current) ||
        !read_float_value("Temperature (C): ",
                          &measurement.temperature_c) ||
        !read_float_value("Measured capacity (Ah): ",
                          &measurement.measured_capacity_ah)) {
        printf("Invalid or incomplete input.\n");
        return;
    }

    status = measurement_add(database, &measurement);
    printf("%s\n", status_code_message(status));

    if (status == STATUS_SUCCESS) {
        status = storage_save(database, BATTERY_FILE, MEASUREMENT_FILE);
        printf("Save: %s\n", status_code_message(status));
    }
}

static void show_battery_analysis(const BatteryDatabase *database)
{
    char battery_id[BATTERY_ID_SIZE];
    int index;
    size_t i;
    unsigned int latest_cycle = 0U;
    float latest_capacity = 0.0f;
    float soh;
    float degradation;
    BatteryHealth health;

    if (!read_line("Battery ID: ", battery_id, sizeof(battery_id))) {
        return;
    }

    index = battery_find_index(database, battery_id);

    if (index < 0) {
        printf("Battery not found.\n");
        return;
    }

    for (i = 0U; i < database->measurement_count; ++i) {
        const BatteryMeasurement *measurement =
            &database->measurements[i];

        if (strcmp(measurement->battery_id, battery_id) == 0 &&
            measurement->cycle_number > latest_cycle) {
            latest_cycle = measurement->cycle_number;
            latest_capacity = measurement->measured_capacity_ah;
        }
    }

    if (latest_cycle == 0U) {
        printf("No measurements available for this battery.\n");
        return;
    }

    if (calculate_soh(
            database->batteries[(size_t)index].rated_capacity_ah,
            latest_capacity, &soh) != STATUS_SUCCESS ||
        calculate_degradation(soh, &degradation) != STATUS_SUCCESS ||
        classify_battery_health(
            soh,
            DEFAULT_HEALTHY_THRESHOLD,
            DEFAULT_DEGRADED_THRESHOLD,
            &health) != STATUS_SUCCESS) {
        printf("Unable to calculate battery health.\n");
        return;
    }

    printf("\n--- BATTERY ANALYSIS ---\n");
    printf("Latest cycle: %u\n", latest_cycle);
    printf("Measured capacity: %.2f Ah\n", latest_capacity);
    printf("SOH: %.2f%%\n", soh);
    printf("Degradation: %.2f%%\n", degradation);
    printf("Health: %s\n", battery_health_name(health));
}

static void show_rul_prediction(const BatteryDatabase *database)
{
    char battery_id[BATTERY_ID_SIZE];
    RULPrediction prediction;
    StatusCode status;

    if (!read_line("Enter Battery ID: ",
                   battery_id, sizeof(battery_id))) {
        return;
    }

    status = predict_remaining_useful_life(
        database,
        battery_id,
        DEFAULT_EOL_THRESHOLD,
        &prediction
    );

    if (status == STATUS_NOT_FOUND) {
        printf("Battery not found.\n");
        return;
    }

    if (status == STATUS_INVALID_DATA) {
        printf("\nPrediction unavailable.\n");
        printf("At least %u valid historical measurements with "
               "different cycle numbers and a decreasing SOH trend "
               "are required.\n",
               (unsigned int)MIN_PREDICTION_POINTS);
        return;
    }

    if (status != STATUS_SUCCESS) {
        printf("Prediction failed: %s\n",
               status_code_message(status));
        return;
    }

    printf("\n===== BATTERY RUL PREDICTION =====\n");
    printf("Battery ID: %s\n", battery_id);
    printf("Historical data points: %zu\n", prediction.data_points);
    printf("Latest cycle: %u\n", prediction.latest_cycle);
    printf("Latest SOH: %.2f%%\n", prediction.latest_soh);

    printf("SOH degradation rate: %.5f%% per cycle\n",
           -prediction.slope);

    printf("Regression R-squared: %.4f\n",
           prediction.r_squared);

    printf("Configured EOL threshold: %.2f%%\n",
           DEFAULT_EOL_THRESHOLD);

    printf("Estimated EOL cycle: %.2f\n",
           prediction.estimated_eol_cycle);

    printf("Estimated remaining cycles: %.2f\n",
           prediction.remaining_cycles);

    printf("\nNote: This is a linear-regression estimate based on "
           "historical measurements, not a guaranteed battery life.\n");
}

int main(void)
{
    BatteryDatabase database;
    char input[INPUT_SIZE];
    int choice = -1;
    StatusCode status;

    database_init(&database);

    status = storage_load(&database, BATTERY_FILE, MEASUREMENT_FILE);

    if (status != STATUS_SUCCESS) {
        printf("Warning: could not load all stored data: %s\n",
               status_code_message(status));
    }

    do {
        printf("\n===== EV BATTERY LIFECYCLE SYSTEM =====\n");
        printf("1. Register battery\n");
        printf("2. Add battery measurement\n");
        printf("3. View battery analysis\n");
        printf("4. List batteries and summary\n");
        printf("5. Generate battery report\n");
        printf("6. Predict remaining useful life (RUL)\n");
        printf("0. Save and exit\n");
        printf("Choice: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        {
            char *end = NULL;
            long parsed = strtol(input, &end, 10);

            if (end == input ||
                (*end != '\n' && *end != '\0')) {
                printf("Invalid menu choice.\n");
                continue;
            }

            if (parsed < 0 || parsed > 6) {
                printf("Unsupported menu choice.\n");
                continue;
            }

            choice = (int)parsed;
        }

        switch (choice) {
            case 1:
                add_battery_menu(&database);
                break;

            case 2:
                add_measurement_menu(&database);
                break;

            case 3:
                show_battery_analysis(&database);
                break;

            case 4:
                report_summary(&database);
                break;

            case 5: {
                char battery_id[BATTERY_ID_SIZE];

                if (read_line("Battery ID: ",
                              battery_id, sizeof(battery_id))) {
                    status = report_battery(
                        &database, battery_id, REPORT_FILE);

                    printf("%s\n", status_code_message(status));

                    if (status == STATUS_SUCCESS) {
                        printf("Report saved to %s\n", REPORT_FILE);
                    }
                }
                break;
            }

            case 6:
                show_rul_prediction(&database);
                break;

            case 0:
                status = storage_save(
                    &database, BATTERY_FILE, MEASUREMENT_FILE);

                printf("Final save: %s\n",
                       status_code_message(status));
                printf("Exiting application.\n");
                break;

            default:
                printf("Unsupported menu choice.\n");
                break;
        }

    } while (choice != 0);

    return 0;
}
