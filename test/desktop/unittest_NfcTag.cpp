#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/NfcTag/NfcTag.h"
#include "mocks/unittest_NfcTag_mocks.h"

using ::testing::Return;    
TEST(NfcTag, basicTest)
{
    EXPECT_EQ(4, 2+2);
}