#ifndef VALIDATION_H
#define VALIDATION_H

#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif

int validate_battery(const Battery *battery);
int validate_measurement(const Battery *battery,
                         const BatteryMeasurement *measurement);
int validate_battery_id(const char *battery_id);

#ifdef __cplusplus
}
#endif

#endif
