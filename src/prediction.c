
#include "prediction.h"
#include "analytics.h"

#include <math.h>
#include <string.h>

StatusCode predict_remaining_useful_life(
    const BatteryDatabase *database,
    const char *battery_id,
    double eol_threshold,
    RULPrediction *result)
{
    size_t i;
    size_t j;
    size_t battery_index = 0U;
    size_t count = 0U;

    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_x2 = 0.0;
    double sum_xy = 0.0;

    double mean_x;
    double mean_y;
    double denominator;

    double slope;
    double intercept;
    double eol_cycle;

    double sse = 0.0;
    double sst = 0.0;

    unsigned int latest_cycle = 0U;
    double latest_soh = 0.0;

    if (database == NULL ||
        battery_id == NULL ||
        result == NULL ||
        battery_id[0] == '\0' ||
        !isfinite(eol_threshold) ||
        eol_threshold <= 0.0 ||
        eol_threshold >= 100.0 ||
        database->battery_count > MAX_BATTERIES ||
        database->measurement_count > MAX_MEASUREMENTS) {
        return STATUS_INVALID_INPUT;
    }

    /* Step 1: Find the requested battery. */
    for (i = 0U; i < database->battery_count; ++i) {
        if (strcmp(database->batteries[i].battery_id,
                   battery_id) == 0) {
            battery_index = i;
            break;
        }
    }

    if (i == database->battery_count) {
        return STATUS_NOT_FOUND;
    }

    {
        const Battery *battery = &database->batteries[battery_index];
        double rated_capacity = (double)battery->rated_capacity_ah;

        if (!isfinite(rated_capacity) || rated_capacity <= 0.0) {
            return STATUS_INVALID_DATA;
        }

        /* Step 2: Collect historical SOH and cycle data. */
        for (i = 0U; i < database->measurement_count; ++i) {
            const BatteryMeasurement *measurement =
                &database->measurements[i];

            float soh_value;
            double x;
            double y;

            if (strcmp(measurement->battery_id, battery_id) != 0) {
                continue;
            }

            if (measurement->cycle_number == 0U ||
                calculate_soh(battery->rated_capacity_ah,
                              measurement->measured_capacity_ah,
                              &soh_value) != STATUS_SUCCESS) {
                return STATUS_INVALID_DATA;
            }

            /* Reject duplicate cycle numbers for this battery. */
            for (j = 0U; j < i; ++j) {
                const BatteryMeasurement *previous =
                    &database->measurements[j];

                if (strcmp(previous->battery_id, battery_id) == 0 &&
                    previous->cycle_number == measurement->cycle_number) {
                    return STATUS_INVALID_DATA;
                }
            }

            x = (double)measurement->cycle_number;
            y = (double)soh_value;

            sum_x += x;
            sum_y += y;
            sum_x2 += x * x;
            sum_xy += x * y;

            ++count;

            if (measurement->cycle_number > latest_cycle) {
                latest_cycle = measurement->cycle_number;
                latest_soh = y;
            }
        }
    }

    /* Step 3: Check whether enough historical data exists. */
    if (count < MIN_PREDICTION_POINTS) {
        return STATUS_INVALID_DATA;
    }

    mean_x = sum_x / (double)count;
    mean_y = sum_y / (double)count;

    denominator = sum_x2 - (sum_x * sum_x / (double)count);

    if (!isfinite(denominator) || denominator <= 1e-12) {
        return STATUS_INVALID_DATA;
    }

    /* Step 4: Calculate linear regression: SOH = intercept + slope * cycle. */
    slope = (sum_xy - (sum_x * sum_y / (double)count)) /
            denominator;

    intercept = mean_y - slope * mean_x;

    if (!isfinite(slope) || !isfinite(intercept)) {
        return STATUS_INVALID_DATA;
    }

    /* Step 5: A non-decreasing SOH trend cannot predict EOL. */
    if (slope >= 0.0) {
        return STATUS_INVALID_DATA;
    }

    /* Step 6: Calculate R-squared to describe the fit. */
    for (i = 0U; i < database->measurement_count; ++i) {
        const BatteryMeasurement *measurement =
            &database->measurements[i];

        double actual_soh;
        double predicted_soh;
        double residual;
        double difference_from_mean;

        if (strcmp(measurement->battery_id, battery_id) != 0) {
            continue;
        }

        actual_soh =
            ((double)measurement->measured_capacity_ah /
             (double)database->batteries[battery_index].rated_capacity_ah)
            * 100.0;

        predicted_soh =
            intercept + slope * (double)measurement->cycle_number;

        residual = actual_soh - predicted_soh;
        difference_from_mean = actual_soh - mean_y;

        sse += residual * residual;
        sst += difference_from_mean * difference_from_mean;
    }

    /* Step 7: Estimate the End-of-Life cycle. */
    eol_cycle = (eol_threshold - intercept) / slope;

    if (!isfinite(eol_cycle) || eol_cycle < 0.0) {
        return STATUS_INVALID_DATA;
    }

    /* Step 8: Store the prediction results. */
    result->data_points = count;
    result->latest_cycle = latest_cycle;
    result->latest_soh = latest_soh;
    result->slope = slope;
    result->intercept = intercept;

    if (sst > 1e-12) {
        result->r_squared = 1.0 - (sse / sst);
    } else {
        result->r_squared = 0.0;
    }

    /* Protect against tiny floating-point rounding errors. */
    if (result->r_squared < 0.0) {
        result->r_squared = 0.0;
    } else if (result->r_squared > 1.0) {
        result->r_squared = 1.0;
    }

    result->estimated_eol_cycle = eol_cycle;

    if (eol_cycle > (double)latest_cycle) {
        result->remaining_cycles =
            eol_cycle - (double)latest_cycle;
    } else {
        result->remaining_cycles = 0.0;
    }

    return STATUS_SUCCESS;
}
