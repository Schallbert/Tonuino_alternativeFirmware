#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ClickEncoder_mocks.h"

#include "ClickEncoder_Abstraction/ClickEncoder_supportsLongPress.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;

class EncoderLongPressRepeatTest : public ::testing::Test
{
protected:
    NiceMock<Mock_ClickEncoder> enc{};
    uint16_t longPressRepeatTicks{6}; 
    Encoder_longPressRepeat m_Enc{Encoder_longPressRepeat(enc, longPressRepeatTicks)};
};

TEST_F(EncoderLongPressRepeatTest, encoderServiceCalled)
{
    EXPECT_CALL(enc, service());

    m_Enc.service();
}

TEST_F(EncoderLongPressRepeatTest, getButton_Open_willNeverReturnLongPressRepeat)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));
    for (uint8_t i = 0; i < longPressRepeatTicks + 10; ++i)
    {
        m_Enc.service();
    }
    ASSERT_EQ(m_Enc.getButton(), Encoder_longPressRepeat::Open);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_notLongEnough_wilReturnHeld)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks; ++i)
    {
        m_Enc.service();
    }
    ASSERT_EQ(m_Enc.getButton(), Encoder_longPressRepeat::Held);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_justLongEnough_wilReturnLongPressRepeat)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks + 1; ++i)
    {
        m_Enc.service();
    }
    ASSERT_EQ(m_Enc.getButton(), Encoder_longPressRepeat::LongPressRepeat);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_longerThanNeeded_wilReturnLongPressRepeat)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks + 5; ++i)
    {
        m_Enc.service();
    }
    ASSERT_EQ(m_Enc.getButton(), Encoder_longPressRepeat::LongPressRepeat);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_longPressIsResetOnRead)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks + 1; ++i)
    {
        m_Enc.service();
    }
    m_Enc.getButton();
    ASSERT_EQ(m_Enc.getButton(), Encoder_longPressRepeat::Held);
}