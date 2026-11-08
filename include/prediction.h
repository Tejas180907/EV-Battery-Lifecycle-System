
#ifndef PREDICTION_H
#define PREDICTION_H

#include "battery.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_PREDICTION_POINTS 3U
#define DEFAULT_EOL_THRESHOLD 80.0

typedef struct {
    size_t data_points;

    unsigned int latest_cycle;
    double latest_soh;

    double slope;
    double intercept;
    double r_squared;

    double estimated_eol_cycle;
    double remaining_cycles;
} RULPrediction;

/*
 * Estimates remaining useful life using linear regression
 * of battery SOH against cycle number.
 *
 * Requires at least MIN_PREDICTION_POINTS measurements.
 */
StatusCode predict_remaining_useful_life(
    const BatteryDatabase *database,
    const char *battery_id,
    double eol_threshold,
    RULPrediction *result
);

#ifdef __cplusplus
}
#endif

#endif
