#include "report.h"
#include "analytics.h"
#include "battery_manager.h"

#include <stdio.h>
#include <string.h>

StatusCode report_battery(const BatteryDatabase *database,
                          const char *battery_id,
                          const char *output_file)
{
    int index;
    size_t i;
    FILE *file;
    unsigned int latest_cycle = 0U;
    float latest_capacity = 0.0f;
    float soh;
    float degradation;
    BatteryHealth health;

    if (database == NULL || battery_id == NULL || output_file == NULL) {
        return STATUS_INVALID_INPUT;
    }

    index = battery_find_index(database, battery_id);
    if (index < 0) {
        return STATUS_NOT_FOUND;
    }

    for (i = 0U; i < database->measurement_count; ++i) {
        const BatteryMeasurement *m = &database->measurements[i];
        if (strcmp(m->battery_id, battery_id) == 0 &&
            m->cycle_number > latest_cycle) {
            latest_cycle = m->cycle_number;
            latest_capacity = m->measured_capacity_ah;
        }
    }

    if (latest_cycle == 0U) {
        return STATUS_NOT_FOUND;
    }

    if (calculate_soh(database->batteries[(size_t)index].rated_capacity_ah,
                      latest_capacity, &soh) != STATUS_SUCCESS ||
        calculate_degradation(soh, &degradation) != STATUS_SUCCESS ||
        classify_battery_health(soh, DEFAULT_HEALTHY_THRESHOLD,
                                DEFAULT_DEGRADED_THRESHOLD, &health) != STATUS_SUCCESS) {
        return STATUS_INVALID_DATA;
    }

    file = fopen(output_file, "w");
    if (file == NULL) {
        return STATUS_FILE_ERROR;
    }

    if (fprintf(file,
                "EV BATTERY LIFECYCLE REPORT\n"
                "Battery ID: %s\n"
                "Model: %s\n"
                "Latest cycle: %u\n"
                "Latest measured capacity: %.2f Ah\n"
                "State of Health: %.2f%%\n"
                "Capacity degradation: %.2f%%\n"
                "Health classification: %s\n",
                database->batteries[(size_t)index].battery_id,
                database->batteries[(size_t)index].model_name,
                latest_cycle, latest_capacity, soh, degradation,
                battery_health_name(health)) < 0) {
        (void)fclose(file);
        return STATUS_FILE_ERROR;
    }

    return fclose(file) == 0 ? STATUS_SUCCESS : STATUS_FILE_ERROR;
}

void report_summary(const BatteryDatabase *database)
{
    size_t i;

    if (database == NULL) {
        return;
    }

    printf("\nBattery records: %zu\n", database->battery_count);
    printf("Measurement records: %zu\n", database->measurement_count);

    for (i = 0U; i < database->battery_count; ++i) {
        const Battery *battery = &database->batteries[i];
        unsigned int latest_cycle = 0U;
        float latest_capacity = 0.0f;
        size_t j;
        float soh;
        BatteryHealth health;

        for (j = 0U; j < database->measurement_count; ++j) {
            const BatteryMeasurement *m = &database->measurements[j];
            if (strcmp(m->battery_id, battery->battery_id) == 0 &&
                m->cycle_number > latest_cycle) {
                latest_cycle = m->cycle_number;
                latest_capacity = m->measured_capacity_ah;
            }
        }

        printf("\nID: %s | Model: %s", battery->battery_id, battery->model_name);
        if (latest_cycle == 0U) {
            printf(" | No measurements yet\n");
        } else if (calculate_soh(battery->rated_capacity_ah,
                                 latest_capacity, &soh) == STATUS_SUCCESS &&
                   classify_battery_health(soh, DEFAULT_HEALTHY_THRESHOLD,
                                           DEFAULT_DEGRADED_THRESHOLD,
                                           &health) == STATUS_SUCCESS) {
            printf(" | Cycle: %u | SOH: %.1f%% | Status: %s\n",
                   latest_cycle, soh, battery_health_name(health));
        } else {
            printf(" | Analysis unavailable\n");
        }
    }
}
