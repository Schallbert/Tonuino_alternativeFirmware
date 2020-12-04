#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ClickEncoder_mocks.h"

#include "../UserInput/ClickEncoder/ClickEncoder_supportsLongPress.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;

class EncoderLongPressRepeatTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pEnc = new Encoder_longPressRepeat(&enc, longPressRepeatTicks);
    }

    virtual void TearDown()
    {
        delete m_pEnc;
    }

protected:
    NiceMock<Mock_ClickEncoder> enc{};
    uint16_t longPressRepeatTicks{6}; 
    Encoder_longPressRepeat *m_pEnc{nullptr};
};

TEST_F(EncoderLongPressRepeatTest, encoderServiceCalled)
{
    EXPECT_CALL(enc, service());

    m_pEnc->service();
}

TEST_F(EncoderLongPressRepeatTest, getButton_Open_willNeverReturnLongPressRepeat)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));
    for (uint8_t i = 0; i < longPressRepeatTicks + 10; ++i)
    {
        m_pEnc->service();
    }
    ASSERT_EQ(m_pEnc->getButton(), Encoder_longPressRepeat::Open);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_notLongEnough_wilReturnHeld)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks; ++i)
    {
        m_pEnc->service();
    }
    ASSERT_EQ(m_pEnc->getButton(), Encoder_longPressRepeat::Held);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_justLongEnough_wilReturnLongPressRepeat)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks + 1; ++i)
    {
        m_pEnc->service();
    }
    ASSERT_EQ(m_pEnc->getButton(), Encoder_longPressRepeat::LongPressRepeat);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_longerThanNeeded_wilReturnLongPressRepeat)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks + 5; ++i)
    {
        m_pEnc->service();
    }
    ASSERT_EQ(m_pEnc->getButton(), Encoder_longPressRepeat::LongPressRepeat);
}

TEST_F(EncoderLongPressRepeatTest, getButton_Held_longPressIsResetOnRead)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for (uint8_t i = 0; i < longPressRepeatTicks + 1; ++i)
    {
        m_pEnc->service();
    }
    m_pEnc->getButton();
    ASSERT_EQ(m_pEnc->getButton(), Encoder_longPressRepeat::Held);
}