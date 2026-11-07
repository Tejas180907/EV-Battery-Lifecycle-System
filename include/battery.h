#ifndef BATTERY_H
#define BATTERY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BATTERIES 100
#define MAX_MEASUREMENTS 1000
#define BATTERY_ID_SIZE 16
#define MODEL_NAME_SIZE 32
#define DATA_PATH_SIZE 256

#define DEFAULT_HEALTHY_THRESHOLD 80.0f
#define DEFAULT_DEGRADED_THRESHOLD 60.0f
#define MIN_ALLOWED_TEMPERATURE_C (-40.0f)
#define MAX_ALLOWED_TEMPERATURE_C 120.0f

typedef enum {
    HEALTHY = 0,
    DEGRADED,
    CRITICAL
} BatteryHealth;

typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_INVALID_INPUT,
    STATUS_NOT_FOUND,
    STATUS_DUPLICATE,
    STATUS_STORAGE_FULL,
    STATUS_FILE_ERROR,
    STATUS_INVALID_DATA
} StatusCode;

typedef struct {
    char battery_id[BATTERY_ID_SIZE];
    char model_name[MODEL_NAME_SIZE];
    float rated_capacity_ah;
    float nominal_voltage;
    float min_voltage;
    float max_voltage;
    float max_temperature_c;
} Battery;

typedef struct {
    char battery_id[BATTERY_ID_SIZE];
    unsigned int cycle_number;
    float voltage;
    float current;
    float temperature_c;
    float measured_capacity_ah;
} BatteryMeasurement;

typedef struct {
    Battery batteries[MAX_BATTERIES];
    size_t battery_count;
    BatteryMeasurement measurements[MAX_MEASUREMENTS];
    size_t measurement_count;
} BatteryDatabase;

void database_init(BatteryDatabase *database);
const char *status_code_message(StatusCode status);
const char *battery_health_name(BatteryHealth health);

#ifdef __cplusplus
}
#endif

#endif
