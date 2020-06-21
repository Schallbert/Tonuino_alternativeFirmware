#include "inc/unittest_StatusLed.h"

namespace statusled
{

// tests
void statusLed_pinActiveHigh_pinActiveLow(void)
{
    bool active = HIGH;
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    TEST_ASSERT_EQUAL(!active, digitalRead(LED_PIN));
    active = LOW;
    aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    TEST_ASSERT_EQUAL(!active, digitalRead(LED_PIN));
}

void statusLed_Init_state_is_off(void)
{
    bool active = HIGH;
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    TEST_ASSERT_EQUAL(!active, digitalRead(LED_PIN));
    aLed.led_service();
    TEST_ASSERT_EQUAL(!active, digitalRead(LED_PIN));
}

void statusLed_off(void)
{
    bool active = HIGH;
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    aLed.set_led_behavior(StatusLed::off);
    aLed.led_service();
    TEST_ASSERT_EQUAL(!active, digitalRead(LED_PIN));
}

void statusLed_solid(void)
{
    bool active = HIGH;
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    aLed.set_led_behavior(StatusLed::solid);
    aLed.led_service();
    TEST_ASSERT_EQUAL(active, digitalRead(LED_PIN));
}

void statusLed_flashSlow(void)
{
    //initialize objects and define behavior
    bool active = HIGH;
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    aLed.set_led_behavior(StatusLed::flash_slow);
    uint16_t i = 0;
    while (i < FLASHSLOWMS - 1)
    {
        //service increments by 1ms each time it is called
        aLed.led_service();
        i++;
    }
    //detect edge of flash
    TEST_ASSERT_EQUAL(!active, digitalRead(LED_PIN));
    aLed.led_service();
    TEST_ASSERT_EQUAL(active, digitalRead(LED_PIN));
}

void statusLed_flashQuick(void)
{
    bool active = HIGH;
    //initialize objects and define behavior
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    aLed.set_led_behavior(StatusLed::flash_quick);
    uint16_t i = 0;
    while (i < FLASHQUICKMS - 1)
    {
        //service increments by 1ms each time it is called
        aLed.led_service();
        i++;
    }
    //detect edge of flash
    TEST_ASSERT_EQUAL(!active, digitalRead(LED_PIN));
    aLed.led_service();
    TEST_ASSERT_EQUAL(active, digitalRead(LED_PIN));
} 

void statusLed_dim(void)
{
    bool active = HIGH;
    //initialize objects and define behavior
    StatusLed aLed = StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, active);
    aLed.set_led_behavior(StatusLed::dim);
    uint8_t i = 0;
    while (i < 25)
    {
        TEST_ASSERT_EQUAL((i & 0x08) & 0x01, digitalRead(LED_PIN));
        i++;
        aLed.led_service();
    }
}

void unittests()
{
    RUN_TEST(statusLed_Init_state_is_off);
    RUN_TEST(statusLed_pinActiveHigh_pinActiveLow);
    RUN_TEST(statusLed_off);
    RUN_TEST(statusLed_solid);
    RUN_TEST(statusLed_flashSlow);
    RUN_TEST(statusLed_flashQuick);
    RUN_TEST(statusLed_dim);
}
} // namespace statusled