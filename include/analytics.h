#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif

StatusCode calculate_soh(float rated_capacity_ah,
                         float measured_capacity_ah,
                         float *soh_percent);
StatusCode calculate_degradation(float soh_percent,
                                 float *degradation_percent);
StatusCode estimate_energy_wh(float voltage,
                              float discharged_capacity_ah,
                              float *energy_wh);
StatusCode classify_battery_health(float soh_percent,
                                   float healthy_threshold,
                                   float degraded_threshold,
                                   BatteryHealth *health);

#ifdef __cplusplus
}
#endif

#endif
