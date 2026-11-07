#include "validation.h"

#include <math.h>
#include <string.h>

int validate_battery_id(const char *battery_id)
{
    size_t i;

    if (battery_id == NULL || battery_id[0] == '\0') {
        return 0;
    }

    if (strlen(battery_id) >= BATTERY_ID_SIZE) {
        return 0;
    }

    for (i = 0U; battery_id[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)battery_id[i];
        if (!(ch >= 'A' && ch <= 'Z') &&
            !(ch >= 'a' && ch <= 'z') &&
            !(ch >= '0' && ch <= '9') &&
            ch != '-' && ch != '_') {
            return 0;
        }
    }

    return 1;
}

int validate_battery(const Battery *battery)
{
    if (battery == NULL ||
        !validate_battery_id(battery->battery_id) ||
        battery->model_name[0] == '\0' ||
        strlen(battery->model_name) >= MODEL_NAME_SIZE) {
        return 0;
    }

    if (!isfinite(battery->rated_capacity_ah) ||
        !isfinite(battery->nominal_voltage) ||
        !isfinite(battery->min_voltage) ||
        !isfinite(battery->max_voltage) ||
        !isfinite(battery->max_temperature_c)) {
        return 0;
    }

    if (battery->rated_capacity_ah <= 0.0f ||
        battery->nominal_voltage <= 0.0f ||
        battery->min_voltage <= 0.0f ||
        battery->max_voltage <= battery->min_voltage ||
        battery->nominal_voltage < battery->min_voltage ||
        battery->nominal_voltage > battery->max_voltage ||
        battery->max_temperature_c < MIN_ALLOWED_TEMPERATURE_C ||
        battery->max_temperature_c > MAX_ALLOWED_TEMPERATURE_C) {
        return 0;
    }

    return 1;
}

int validate_measurement(const Battery *battery,
                         const BatteryMeasurement *measurement)
{
    if (battery == NULL || measurement == NULL) {
        return 0;
    }

    if (strcmp(battery->battery_id, measurement->battery_id) != 0 ||
        measurement->cycle_number == 0U) {
        return 0;
    }

    if (!isfinite(measurement->voltage) ||
        !isfinite(measurement->current) ||
        !isfinite(measurement->temperature_c) ||
        !isfinite(measurement->measured_capacity_ah)) {
        return 0;
    }

    if (measurement->voltage < battery->min_voltage ||
        measurement->voltage > battery->max_voltage ||
        measurement->current < 0.0f ||
        measurement->temperature_c < MIN_ALLOWED_TEMPERATURE_C ||
        measurement->temperature_c > battery->max_temperature_c ||
        measurement->measured_capacity_ah <= 0.0f ||
        measurement->measured_capacity_ah > battery->rated_capacity_ah) {
        return 0;
    }

    return 1;
}
