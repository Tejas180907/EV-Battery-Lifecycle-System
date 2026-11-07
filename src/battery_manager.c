#include "battery_manager.h"
#include "validation.h"

#include <string.h>

StatusCode battery_add(BatteryDatabase *database, const Battery *battery)
{
    if (database == NULL || !validate_battery(battery)) {
        return STATUS_INVALID_INPUT;
    }

    if (database->battery_count >= MAX_BATTERIES) {
        return STATUS_STORAGE_FULL;
    }

    if (battery_find_index(database, battery->battery_id) >= 0) {
        return STATUS_DUPLICATE;
    }

    database->batteries[database->battery_count] = *battery;
    database->battery_count++;
    return STATUS_SUCCESS;
}

int battery_find_index(const BatteryDatabase *database, const char *battery_id)
{
    size_t i;

    if (database == NULL || battery_id == NULL || battery_id[0] == '\0') {
        return -1;
    }

    for (i = 0U; i < database->battery_count; ++i) {
        if (strcmp(database->batteries[i].battery_id, battery_id) == 0) {
            return (int)i;
        }
    }

    return -1;
}

StatusCode battery_update(BatteryDatabase *database, const char *battery_id,
                          const Battery *updated_battery)
{
    int index;

    if (database == NULL || battery_id == NULL ||
        !validate_battery(updated_battery) ||
        strcmp(battery_id, updated_battery->battery_id) != 0) {
        return STATUS_INVALID_INPUT;
    }

    index = battery_find_index(database, battery_id);
    if (index < 0) {
        return STATUS_NOT_FOUND;
    }

    database->batteries[(size_t)index] = *updated_battery;
    return STATUS_SUCCESS;
}

StatusCode measurement_add(BatteryDatabase *database,
                           const BatteryMeasurement *measurement)
{
    int battery_index;
    size_t i;
    unsigned int latest_cycle = 0U;

    if (database == NULL || measurement == NULL) {
        return STATUS_INVALID_INPUT;
    }

    battery_index = battery_find_index(database, measurement->battery_id);
    if (battery_index < 0) {
        return STATUS_NOT_FOUND;
    }

    if (!validate_measurement(&database->batteries[(size_t)battery_index],
                              measurement)) {
        return STATUS_INVALID_INPUT;
    }

    for (i = 0U; i < database->measurement_count; ++i) {
        const BatteryMeasurement *existing = &database->measurements[i];

        if (strcmp(existing->battery_id, measurement->battery_id) == 0) {
            if (existing->cycle_number == measurement->cycle_number) {
                return STATUS_DUPLICATE;
            }
            if (existing->cycle_number > latest_cycle) {
                latest_cycle = existing->cycle_number;
            }
        }
    }

    if (measurement->cycle_number <= latest_cycle) {
        return STATUS_INVALID_INPUT;
    }

    if (database->measurement_count >= MAX_MEASUREMENTS) {
        return STATUS_STORAGE_FULL;
    }

    database->measurements[database->measurement_count] = *measurement;
    database->measurement_count++;
    return STATUS_SUCCESS;
}
