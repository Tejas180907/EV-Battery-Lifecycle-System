#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif

StatusCode battery_add(BatteryDatabase *database, const Battery *battery);
int battery_find_index(const BatteryDatabase *database, const char *battery_id);
StatusCode battery_update(BatteryDatabase *database, const char *battery_id,
                          const Battery *updated_battery);
StatusCode measurement_add(BatteryDatabase *database,
                           const BatteryMeasurement *measurement);

#ifdef __cplusplus
}
#endif

#endif
