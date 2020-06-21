#ifdef UNIT_TEST
#include <gtest/gtest.h> //for unittest
#include <gmock/gmock.h>
//#include "unittest_Folder.h"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}

#endif