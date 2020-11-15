#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SimpleTimer.h>

TEST(simpleTimer, timerNotSet_doesNotElapse)
{
    SimpleTimer testTimer;
    EXPECT_FALSE(testTimer.isElapsed());
    testTimer.timerTick();
    EXPECT_FALSE(testTimer.isElapsed());
}

TEST(simpleTimer, timerStart_thresholdReached_Elapses)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    EXPECT_FALSE(testTimer.isElapsed());
    testTimer.timerTick();
    EXPECT_TRUE(testTimer.isElapsed());
}

TEST(simpleTimer, timerStart_overSaturated_Elapsed)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.timerTick();
    testTimer.timerTick();
    EXPECT_TRUE(testTimer.isElapsed());
}

TEST(simpleTimer, timerElapsed_Stop_elapseDeleted)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.timerTick();
    testTimer.stop();
    EXPECT_FALSE(testTimer.isElapsed());
}

TEST(simpleTimer, timerStop_doesNotElapse)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.stop();
    testTimer.timerTick();
    testTimer.timerTick();
    EXPECT_FALSE(testTimer.isElapsed());
}

TEST(simpleTimer, timerStart_updateThreshold_notElapsed)
{
    SimpleTimer testTimer;
    testTimer.start(2);
    testTimer.timerTick();
    testTimer.start(3);
    testTimer.timerTick();
    EXPECT_FALSE(testTimer.isElapsed());
    testTimer.timerTick();
    EXPECT_TRUE(testTimer.isElapsed());
}

TEST(simpleTimer, timerStart_elapses_updateThreshold_staysElapsed)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.timerTick();
    EXPECT_TRUE(testTimer.isElapsed());
    testTimer.start(3);
    EXPECT_TRUE(testTimer.isElapsed()); 
}