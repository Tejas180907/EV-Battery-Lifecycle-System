
#include <gtest/gtest.h>

extern "C" {
#include "prediction.h"
#include "battery.h"
}

#include <cstring>

class PredictionTest : public ::testing::Test {
protected:
    BatteryDatabase database{};

    void SetUp() override {
        database_init(&database);

        database.battery_count = 1;

        std::strcpy(database.batteries[0].battery_id, "EV001");
        std::strcpy(database.batteries[0].model_name, "Test Battery");

        database.batteries[0].rated_capacity_ah = 100.0f;
        database.batteries[0].nominal_voltage = 350.0f;
        database.batteries[0].min_voltage = 280.0f;
        database.batteries[0].max_voltage = 420.0f;
        database.batteries[0].max_temperature_c = 60.0f;
    }

    void AddMeasurement(unsigned int cycle, float capacity) {
        BatteryMeasurement &measurement =
            database.measurements[database.measurement_count];

        std::strcpy(measurement.battery_id, "EV001");

        measurement.cycle_number = cycle;
        measurement.voltage = 365.0f;
        measurement.current = 20.0f;
        measurement.temperature_c = 32.0f;
        measurement.measured_capacity_ah = capacity;

        database.measurement_count++;
    }
};

/* Test 1: Correct linear regression prediction. */
TEST_F(PredictionTest, CalculatesRemainingCyclesCorrectly) {
    AddMeasurement(100, 95.0f);
    AddMeasurement(200, 90.0f);
    AddMeasurement(300, 85.0f);

    RULPrediction result{};

    StatusCode status = predict_remaining_useful_life(
        &database, "EV001", 80.0, &result
    );

    EXPECT_EQ(status, STATUS_SUCCESS);
    EXPECT_NEAR(result.slope, -0.05, 0.0001);
    EXPECT_NEAR(result.estimated_eol_cycle, 400.0, 0.01);
    EXPECT_NEAR(result.remaining_cycles, 100.0, 0.01);
    EXPECT_NEAR(result.r_squared, 1.0, 0.0001);
}

/* Test 2: Insufficient historical measurements. */
TEST_F(PredictionTest, RejectsInsufficientMeasurements) {
    AddMeasurement(100, 95.0f);
    AddMeasurement(200, 90.0f);

    RULPrediction result{};

    EXPECT_EQ(
        predict_remaining_useful_life(
            &database, "EV001", 80.0, &result
        ),
        STATUS_INVALID_DATA
    );
}

/* Test 3: Battery ID does not exist. */
TEST_F(PredictionTest, RejectsUnknownBattery) {
    RULPrediction result{};

    EXPECT_EQ(
        predict_remaining_useful_life(
            &database, "EV999", 80.0, &result
        ),
        STATUS_NOT_FOUND
    );
}

/* Test 4: Invalid EOL threshold. */
TEST_F(PredictionTest, RejectsInvalidEolThreshold) {
    RULPrediction result{};

    EXPECT_EQ(
        predict_remaining_useful_life(
            &database, "EV001", 100.0, &result
        ),
        STATUS_INVALID_INPUT
    );
}

/* Test 5: Increasing SOH cannot predict degradation-based EOL. */
TEST_F(PredictionTest, RejectsIncreasingSohTrend) {
    AddMeasurement(100, 85.0f);
    AddMeasurement(200, 90.0f);
    AddMeasurement(300, 95.0f);

    RULPrediction result{};

    EXPECT_EQ(
        predict_remaining_useful_life(
            &database, "EV001", 80.0, &result
        ),
        STATUS_INVALID_DATA
    );
}

/* Test 6: Duplicate cycle numbers are rejected. */
TEST_F(PredictionTest, RejectsDuplicateCycles) {
    AddMeasurement(100, 95.0f);
    AddMeasurement(100, 92.0f);
    AddMeasurement(300, 85.0f);

    RULPrediction result{};

    EXPECT_EQ(
        predict_remaining_useful_life(
            &database, "EV001", 80.0, &result
        ),
        STATUS_INVALID_DATA
    );
}

/* Test 7: Null database is rejected. */
TEST_F(PredictionTest, RejectsNullDatabase) {
    RULPrediction result{};

    EXPECT_EQ(
        predict_remaining_useful_life(
            nullptr, "EV001", 80.0, &result
        ),
        STATUS_INVALID_INPUT
    );
}

/* Test 8: Null result pointer is rejected. */
TEST_F(PredictionTest, RejectsNullResult) {
    AddMeasurement(100, 95.0f);
    AddMeasurement(200, 90.0f);
    AddMeasurement(300, 85.0f);

    EXPECT_EQ(
        predict_remaining_useful_life(
            &database, "EV001", 80.0, nullptr
        ),
        STATUS_INVALID_INPUT
    );
}
