#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../UserInput/ClickEncoder_implementation/ClickEncoder_supportsLongPress.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;

class EncoderLongPressRepeatTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pEnc = new Encoder_longPressRepeat(&enc, EncoderLongPressRepeatTest);
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

TEST_F(EncoderLongPressRepeatTest, encoderServiceCalled)
{
    EXPECT_CALL(enc, service());

    m_pEnc->service();
}