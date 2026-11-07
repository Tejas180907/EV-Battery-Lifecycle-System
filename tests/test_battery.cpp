#include <gtest/gtest.h>
extern "C" {
#include "battery.h"
#include "battery_manager.h"
}

static Battery makeBattery(const char *id)
{
    Battery battery = {};
    (void)snprintf(battery.battery_id, sizeof(battery.battery_id), "%s", id);
    (void)snprintf(battery.model_name, sizeof(battery.model_name), "%s", "TestPack");
    battery.rated_capacity_ah = 100.0f;
    battery.nominal_voltage = 48.0f;
    battery.min_voltage = 40.0f;
    battery.max_voltage = 54.0f;
    battery.max_temperature_c = 60.0f;
    return battery;
}

TEST(BatteryManagerTest, AddsValidBattery)
{
    BatteryDatabase db;
    database_init(&db);
    Battery battery = makeBattery("EV001");
    EXPECT_EQ(battery_add(&db, &battery), STATUS_SUCCESS);
    EXPECT_EQ(db.battery_count, 1U);
}

TEST(BatteryManagerTest, RejectsDuplicateBatteryId)
{
    BatteryDatabase db;
    database_init(&db);
    Battery battery = makeBattery("EV001");
    EXPECT_EQ(battery_add(&db, &battery), STATUS_SUCCESS);
    EXPECT_EQ(battery_add(&db, &battery), STATUS_DUPLICATE);
}

TEST(BatteryManagerTest, FindsExistingBattery)
{
    BatteryDatabase db;
    database_init(&db);
    Battery battery = makeBattery("EV001");
    ASSERT_EQ(battery_add(&db, &battery), STATUS_SUCCESS);
    EXPECT_EQ(battery_find_index(&db, "EV001"), 0);
}

TEST(BatteryManagerTest, ReturnsMinusOneForMissingBattery)
{
    BatteryDatabase db;
    database_init(&db);
    EXPECT_EQ(battery_find_index(&db, "MISSING"), -1);
}

TEST(BatteryManagerTest, RejectsNullDatabase)
{
    Battery battery = makeBattery("EV001");
    EXPECT_EQ(battery_add(nullptr, &battery), STATUS_INVALID_INPUT);
}
