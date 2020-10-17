#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ClickEncoder_mocks.h"

#include "../UserInput/ClickEncoder_implementation/ClickEncoder_supportsLongPress.h"
#include "../UserInput/UserInput_implementation/UserInput_implementation.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;

class UserInput_3ButtonsTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pUserInput = new UserInput_3Buttons(&plPs, &next, &prev, longPressRepeatTicks);
    }

    virtual void TearDown()
    {
        delete m_pUserInput;
    }

protected:
    NiceMock<Mock_ClickEncoder> plPs{};
    NiceMock<Mock_ClickEncoder> next{};
    NiceMock<Mock_ClickEncoder> prev{};
    uint16_t longPressRepeatTicks{6}; 
    UserInput_3Buttons *m_pUserInput{nullptr};
};

TEST_F(UserInput_3ButtonsTest, ServiceCalled)
{
    EXPECT_CALL(plPs, service());
    EXPECT_CALL(next, service());
    EXPECT_CALL(prev, service());

    m_pUserInput->userinput_service_isr();
}
