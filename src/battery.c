#include "battery.h"

void database_init(BatteryDatabase *database)
{
    if (database == NULL) {
        return;
    }

    database->battery_count = 0U;
    database->measurement_count = 0U;
}

const char *status_code_message(StatusCode status)
{
    switch (status) {
        case STATUS_SUCCESS: return "Success";
        case STATUS_INVALID_INPUT: return "Invalid input";
        case STATUS_NOT_FOUND: return "Record not found";
        case STATUS_DUPLICATE: return "Duplicate record";
        case STATUS_STORAGE_FULL: return "Storage capacity reached";
        case STATUS_FILE_ERROR: return "File operation failed";
        case STATUS_INVALID_DATA: return "Invalid stored data";
        default: return "Unknown status";
    }
}

const char *battery_health_name(BatteryHealth health)
{
    switch (health) {
        case HEALTHY: return "Healthy";
        case DEGRADED: return "Degraded";
        case CRITICAL: return "Critical";
        default: return "Unknown";
    }
}
