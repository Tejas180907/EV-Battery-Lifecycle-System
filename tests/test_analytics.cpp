#include <gtest/gtest.h>
extern "C" {
#include "analytics.h"
}

TEST(AnalyticsTest, CalculatesExpectedSoh)
{
    float soh = 0.0f;
    EXPECT_EQ(calculate_soh(100.0f, 85.0f, &soh), STATUS_SUCCESS);
    EXPECT_FLOAT_EQ(soh, 85.0f);
}

TEST(AnalyticsTest, FullCapacityProducesOneHundredPercentSoh)
{
    float soh = 0.0f;
    EXPECT_EQ(calculate_soh(100.0f, 100.0f, &soh), STATUS_SUCCESS);
    EXPECT_FLOAT_EQ(soh, 100.0f);
}

TEST(AnalyticsTest, ZeroRatedCapacityIsRejected)
{
    float soh = 0.0f;
    EXPECT_EQ(calculate_soh(0.0f, 50.0f, &soh), STATUS_INVALID_INPUT);
}

TEST(AnalyticsTest, MeasuredCapacityAboveRatedCapacityIsRejected)
{
    float soh = 0.0f;
    EXPECT_EQ(calculate_soh(100.0f, 101.0f, &soh), STATUS_INVALID_INPUT);
}

TEST(AnalyticsTest, NullSohOutputIsRejected)
{
    EXPECT_EQ(calculate_soh(100.0f, 80.0f, nullptr), STATUS_INVALID_INPUT);
}

TEST(AnalyticsTest, CalculatesDegradation)
{
    float degradation = 0.0f;
    EXPECT_EQ(calculate_degradation(85.0f, &degradation), STATUS_SUCCESS);
    EXPECT_FLOAT_EQ(degradation, 15.0f);
}

TEST(AnalyticsTest, RejectsSohOutsideRange)
{
    float degradation = 0.0f;
    EXPECT_EQ(calculate_degradation(105.0f, &degradation), STATUS_INVALID_INPUT);
}

TEST(AnalyticsTest, ClassifiesHealthyBoundary)
{
    BatteryHealth health = CRITICAL;
    EXPECT_EQ(classify_battery_health(80.0f, 80.0f, 60.0f, &health), STATUS_SUCCESS);
    EXPECT_EQ(health, HEALTHY);
}

TEST(AnalyticsTest, ClassifiesDegradedBattery)
{
    BatteryHealth health = HEALTHY;
    EXPECT_EQ(classify_battery_health(75.0f, 80.0f, 60.0f, &health), STATUS_SUCCESS);
    EXPECT_EQ(health, DEGRADED);
}

TEST(AnalyticsTest, ClassifiesCriticalBattery)
{
    BatteryHealth health = HEALTHY;
    EXPECT_EQ(classify_battery_health(59.0f, 80.0f, 60.0f, &health), STATUS_SUCCESS);
    EXPECT_EQ(health, CRITICAL);
}

TEST(AnalyticsTest, EstimatesEnergy)
{
    float energy = 0.0f;
    EXPECT_EQ(estimate_energy_wh(48.0f, 2.0f, &energy), STATUS_SUCCESS);
    EXPECT_FLOAT_EQ(energy, 96.0f);
}
