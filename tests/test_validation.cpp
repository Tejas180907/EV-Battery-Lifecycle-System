#include <gtest/gtest.h>
extern "C" {
#include "validation.h"
}

static Battery makeBattery()
{
    Battery battery = {};
    (void)snprintf(battery.battery_id, sizeof(battery.battery_id), "%s", "EV001");
    (void)snprintf(battery.model_name, sizeof(battery.model_name), "%s", "TestPack");
    battery.rated_capacity_ah = 100.0f;
    battery.nominal_voltage = 48.0f;
    battery.min_voltage = 40.0f;
    battery.max_voltage = 54.0f;
    battery.max_temperature_c = 60.0f;
    return battery;
}

TEST(ValidationTest, AcceptsValidBattery)
{
    Battery battery = makeBattery();
    EXPECT_EQ(validate_battery(&battery), 1);
}

TEST(ValidationTest, RejectsEmptyBatteryId)
{
    Battery battery = makeBattery();
    battery.battery_id[0] = '\0';
    EXPECT_EQ(validate_battery(&battery), 0);
}

TEST(ValidationTest, RejectsInvalidVoltageRange)
{
    Battery battery = makeBattery();
    battery.min_voltage = 55.0f;
    EXPECT_EQ(validate_battery(&battery), 0);
}

TEST(ValidationTest, AcceptsValidMeasurement)
{
    Battery battery = makeBattery();
    BatteryMeasurement measurement = {};
    (void)snprintf(measurement.battery_id, sizeof(measurement.battery_id), "%s", "EV001");
    measurement.cycle_number = 1U;
    measurement.voltage = 48.0f;
    measurement.current = 10.0f;
    measurement.temperature_c = 30.0f;
    measurement.measured_capacity_ah = 95.0f;
    EXPECT_EQ(validate_measurement(&battery, &measurement), 1);
}

TEST(ValidationTest, RejectsTemperatureAboveBatteryLimit)
{
    Battery battery = makeBattery();
    BatteryMeasurement measurement = {};
    (void)snprintf(measurement.battery_id, sizeof(measurement.battery_id), "%s", "EV001");
    measurement.cycle_number = 1U;
    measurement.voltage = 48.0f;
    measurement.current = 10.0f;
    measurement.temperature_c = 65.0f;
    measurement.measured_capacity_ah = 95.0f;
    EXPECT_EQ(validate_measurement(&battery, &measurement), 0);
}
