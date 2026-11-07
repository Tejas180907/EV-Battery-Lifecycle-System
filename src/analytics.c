#include "analytics.h"

#include <math.h>

StatusCode calculate_soh(float rated_capacity_ah,
                         float measured_capacity_ah,
                         float *soh_percent)
{
    if (soh_percent == NULL ||
        !isfinite(rated_capacity_ah) ||
        !isfinite(measured_capacity_ah) ||
        rated_capacity_ah <= 0.0f ||
        measured_capacity_ah <= 0.0f ||
        measured_capacity_ah > rated_capacity_ah) {
        return STATUS_INVALID_INPUT;
    }

    *soh_percent = (measured_capacity_ah / rated_capacity_ah) * 100.0f;
    return STATUS_SUCCESS;
}

StatusCode calculate_degradation(float soh_percent,
                                 float *degradation_percent)
{
    if (degradation_percent == NULL ||
        !isfinite(soh_percent) ||
        soh_percent < 0.0f || soh_percent > 100.0f) {
        return STATUS_INVALID_INPUT;
    }

    *degradation_percent = 100.0f - soh_percent;
    return STATUS_SUCCESS;
}

StatusCode estimate_energy_wh(float voltage,
                              float discharged_capacity_ah,
                              float *energy_wh)
{
    if (energy_wh == NULL || !isfinite(voltage) ||
        !isfinite(discharged_capacity_ah) ||
        voltage <= 0.0f || discharged_capacity_ah < 0.0f) {
        return STATUS_INVALID_INPUT;
    }

    *energy_wh = voltage * discharged_capacity_ah;
    return STATUS_SUCCESS;
}

StatusCode classify_battery_health(float soh_percent,
                                   float healthy_threshold,
                                   float degraded_threshold,
                                   BatteryHealth *health)
{
    if (health == NULL || !isfinite(soh_percent) ||
        !isfinite(healthy_threshold) || !isfinite(degraded_threshold) ||
        soh_percent < 0.0f || soh_percent > 100.0f ||
        healthy_threshold <= degraded_threshold ||
        degraded_threshold < 0.0f || healthy_threshold > 100.0f) {
        return STATUS_INVALID_INPUT;
    }

    if (soh_percent >= healthy_threshold) {
        *health = HEALTHY;
    } else if (soh_percent >= degraded_threshold) {
        *health = DEGRADED;
    } else {
        *health = CRITICAL;
    }

    return STATUS_SUCCESS;
}
