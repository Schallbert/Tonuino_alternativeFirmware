#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SimpleTimer.h>

TEST(simpleTimer, timerNotSet_doesNotElapse)
{
    SimpleTimer testTimer;
    EXPECT_FALSE(testTimer.is_elapsed());
    testTimer.timer_tick();
    EXPECT_FALSE(testTimer.is_elapsed());
}

TEST(simpleTimer, timerStart_thresholdReached_notElapsed)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    EXPECT_FALSE(testTimer.is_elapsed());
    testTimer.timer_tick();
    EXPECT_FALSE(testTimer.is_elapsed());
}

TEST(simpleTimer, timerStart_elapsesOK)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.timer_tick();
    testTimer.timer_tick();
    EXPECT_TRUE(testTimer.is_elapsed());
}

TEST(simpleTimer, timerElapsed_Stop_elapseDeleted)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.timer_tick();
    testTimer.stop();
    EXPECT_FALSE(testTimer.is_elapsed());
}

TEST(simpleTimer, timerStop_doesNotElapse)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.stop();
    testTimer.timer_tick();
    testTimer.timer_tick();
    EXPECT_FALSE(testTimer.is_elapsed());
}

TEST(simpleTimer, timerStart_updateThreshold_notElapsed)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.timer_tick();
    testTimer.start(3);
    testTimer.timer_tick();
    testTimer.timer_tick();
    EXPECT_FALSE(testTimer.is_elapsed());
    testTimer.timer_tick();
    EXPECT_TRUE(testTimer.is_elapsed());
}

TEST(simpleTimer, timerStart_elapses_updateThreshold_staysElapsed)
{
    SimpleTimer testTimer;
    testTimer.start(1);
    testTimer.timer_tick();
    testTimer.timer_tick();
    EXPECT_TRUE(testTimer.is_elapsed());
    testTimer.start(3);
    EXPECT_TRUE(testTimer.is_elapsed()); 
}