#ifdef UNIT_TEST
#include <unity.h> //for unittest
#include "unittest_Folder.h"

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
   folder::unittests();

    UNITY_END(); // stop unit testing
}
#endif