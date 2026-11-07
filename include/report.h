#ifndef REPORT_H
#define REPORT_H

#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif

StatusCode report_battery(const BatteryDatabase *database,
                          const char *battery_id,
                          const char *output_file);
void report_summary(const BatteryDatabase *database);

#ifdef __cplusplus
}
#endif

#endif
