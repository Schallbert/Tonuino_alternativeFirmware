
#include <unity.h> //for unittest
#include "unittest_StatusLed.h"
#include "unittest_UserInput.h"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void setup()
{
    delay(2000);
    UNITY_BEGIN();    // IMPORTANT LINE!
}


void loop() 
{
   statusled::unittests();
   userinput::unittests();

    UNITY_END(); // stop unit testing
}