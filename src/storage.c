#include "storage.h"
#include "validation.h"
#include "battery_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static StatusCode ensure_parent_directory_file(const char *path, FILE **file,
                                               const char *mode)
{
    *file = fopen(path, mode);
    if (*file == NULL) {
        return STATUS_FILE_ERROR;
    }
    return STATUS_SUCCESS;
}

StatusCode storage_save(const BatteryDatabase *database,
                        const char *battery_file,
                        const char *measurement_file)
{
    FILE *file;
    size_t i;

    if (database == NULL || battery_file == NULL || measurement_file == NULL) {
        return STATUS_INVALID_INPUT;
    }

    if (ensure_parent_directory_file(battery_file, &file, "w") != STATUS_SUCCESS) {
        return STATUS_FILE_ERROR;
    }

    if (fprintf(file, "battery_id,model_name,rated_capacity_ah,nominal_voltage,min_voltage,max_voltage,max_temperature_c\n") < 0) {
        (void)fclose(file);
        return STATUS_FILE_ERROR;
    }

    for (i = 0U; i < database->battery_count; ++i) {
        const Battery *b = &database->batteries[i];
        if (fprintf(file, "%s,%s,%.3f,%.3f,%.3f,%.3f,%.3f\n",
                    b->battery_id, b->model_name, b->rated_capacity_ah,
                    b->nominal_voltage, b->min_voltage, b->max_voltage,
                    b->max_temperature_c) < 0) {
            (void)fclose(file);
            return STATUS_FILE_ERROR;
        }
    }

    if (fclose(file) != 0) {
        return STATUS_FILE_ERROR;
    }

    if (ensure_parent_directory_file(measurement_file, &file, "w") != STATUS_SUCCESS) {
        return STATUS_FILE_ERROR;
    }

    if (fprintf(file, "battery_id,cycle_number,voltage,current,temperature_c,measured_capacity_ah\n") < 0) {
        (void)fclose(file);
        return STATUS_FILE_ERROR;
    }

    for (i = 0U; i < database->measurement_count; ++i) {
        const BatteryMeasurement *m = &database->measurements[i];
        if (fprintf(file, "%s,%u,%.3f,%.3f,%.3f,%.3f\n",
                    m->battery_id, m->cycle_number, m->voltage, m->current,
                    m->temperature_c, m->measured_capacity_ah) < 0) {
            (void)fclose(file);
            return STATUS_FILE_ERROR;
        }
    }

    return fclose(file) == 0 ? STATUS_SUCCESS : STATUS_FILE_ERROR;
}

static int parse_battery_line(char *line, Battery *battery)
{
    char *token;
    char *save = NULL;
    char *fields[7];
    size_t count = 0U;

    token = strtok_r(line, ",\r\n", &save);
    while (token != NULL && count < 7U) {
        fields[count++] = token;
        token = strtok_r(NULL, ",\r\n", &save);
    }
    if (count != 7U || token != NULL) {
        return 0;
    }

    if (strlen(fields[0]) >= BATTERY_ID_SIZE ||
        strlen(fields[1]) >= MODEL_NAME_SIZE) {
        return 0;
    }

    (void)snprintf(battery->battery_id, sizeof(battery->battery_id), "%s", fields[0]);
    (void)snprintf(battery->model_name, sizeof(battery->model_name), "%s", fields[1]);

    {
        char *end = NULL;
        battery->rated_capacity_ah = strtof(fields[2], &end);
        if (end == fields[2] || *end != '\0') return 0;
        battery->nominal_voltage = strtof(fields[3], &end);
        if (end == fields[3] || *end != '\0') return 0;
        battery->min_voltage = strtof(fields[4], &end);
        if (end == fields[4] || *end != '\0') return 0;
        battery->max_voltage = strtof(fields[5], &end);
        if (end == fields[5] || *end != '\0') return 0;
        battery->max_temperature_c = strtof(fields[6], &end);
        if (end == fields[6] || *end != '\0') return 0;
    }

    return validate_battery(battery);
}

static int parse_measurement_line(char *line, BatteryMeasurement *measurement)
{
    char *token;
    char *save = NULL;
    char *fields[6];
    size_t count = 0U;
    char *end = NULL;
    unsigned long cycle;

    token = strtok_r(line, ",\r\n", &save);
    while (token != NULL && count < 6U) {
        fields[count++] = token;
        token = strtok_r(NULL, ",\r\n", &save);
    }
    if (count != 6U || token != NULL ||
        strlen(fields[0]) >= BATTERY_ID_SIZE) {
        return 0;
    }

    (void)snprintf(measurement->battery_id, sizeof(measurement->battery_id),
                   "%s", fields[0]);
    cycle = strtoul(fields[1], &end, 10);
    if (end == fields[1] || *end != '\0' || cycle == 0UL ||
        cycle > 4294967295UL) return 0;
    measurement->cycle_number = (unsigned int)cycle;

    measurement->voltage = strtof(fields[2], &end);
    if (end == fields[2] || *end != '\0') return 0;
    measurement->current = strtof(fields[3], &end);
    if (end == fields[3] || *end != '\0') return 0;
    measurement->temperature_c = strtof(fields[4], &end);
    if (end == fields[4] || *end != '\0') return 0;
    measurement->measured_capacity_ah = strtof(fields[5], &end);
    if (end == fields[5] || *end != '\0') return 0;

    return 1;
}

StatusCode storage_load(BatteryDatabase *database,
                        const char *battery_file,
                        const char *measurement_file)
{
    FILE *file;
    char line[512];

    if (database == NULL || battery_file == NULL || measurement_file == NULL) {
        return STATUS_INVALID_INPUT;
    }

    database_init(database);

    file = fopen(battery_file, "r");
    if (file != NULL) {
        (void)fgets(line, sizeof(line), file); /* Header */
        while (fgets(line, sizeof(line), file) != NULL) {
            Battery battery;
            if (database->battery_count >= MAX_BATTERIES) {
                (void)fclose(file);
                return STATUS_STORAGE_FULL;
            }
            if (parse_battery_line(line, &battery) &&
                battery_find_index(database, battery.battery_id) < 0) {
                database->batteries[database->battery_count++] = battery;
            }
        }
        if (ferror(file) || fclose(file) != 0) {
            return STATUS_FILE_ERROR;
        }
    } else if (fopen(battery_file, "r") == NULL) {
        /* Missing files are treated as an empty initial database. */
    }

    file = fopen(measurement_file, "r");
    if (file != NULL) {
        (void)fgets(line, sizeof(line), file); /* Header */
        while (fgets(line, sizeof(line), file) != NULL) {
            BatteryMeasurement measurement;
            int index;
            if (database->measurement_count >= MAX_MEASUREMENTS) {
                (void)fclose(file);
                return STATUS_STORAGE_FULL;
            }
            if (parse_measurement_line(line, &measurement)) {
                index = battery_find_index(database, measurement.battery_id);
                if (index >= 0 &&
                    validate_measurement(&database->batteries[(size_t)index],
                                         &measurement)) {
                    database->measurements[database->measurement_count++] = measurement;
                }
            }
        }
        if (ferror(file) || fclose(file) != 0) {
            return STATUS_FILE_ERROR;
        }
    }

    return STATUS_SUCCESS;
}
