
#include <Arduino.h>
#include <unity.h> //for unittest
#include <Arduino_config.h>
#include <Tonuino_config.h>
#include <StatusLed.h>

namespace statusled
{

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

// tests
void statusLed_pinActiveHigh_pinActiveLow(void);
void statusLed_Init_state_is_off(void);
void statusLed_off(void);
void statusLed_solid(void);
void statusLed_flashSlow(void);
void statusLed_flashQuick(void);
void statusLed_dim(void);

void unittests();
} // namespace statusled
