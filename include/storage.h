#ifndef STORAGE_H
#define STORAGE_H

#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif

StatusCode storage_load(BatteryDatabase *database,
                        const char *battery_file,
                        const char *measurement_file);
StatusCode storage_save(const BatteryDatabase *database,
                        const char *battery_file,
                        const char *measurement_file);

#ifdef __cplusplus
}
#endif

#endif
