#ifndef UNIT_TEST
#define UNIT_TEST

//includes
// MAKE SURE PLATFORMIO HAS SET lib_deps = googletest
#include "Arduino.h"
//#include <gtest/gmock.h>
#include <gtest/gtest.h>
#include <StatusLed.h>

#define LED_PIN 
#define FLASHSLOWMS 200
#define FLASHQUICKMS 100

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(StatusLedTest, Init_state_is_off)
{   
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, HIGH);
    ASSERT_EQ(LOW, digitalRead(LED_PIN));
}

TEST(StatusLedTest, DISABLED_Led_Off)
{
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, HIGH);
    aLed.set_led_behavior(StatusLed::off);
    aLed.led_service();
    ASSERT_EQ(LOW, digitalRead(LED_PIN));
}

#endif // UNIT_TEST

