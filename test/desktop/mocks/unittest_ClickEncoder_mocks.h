#ifndef CLICKENCODER_MOCKS_H
#define CLICKENCODER_MOCKS_H

#include <gmock/gmock.h>

#include "../UserInput/ClickEncoder/ClickEncoder_interface.h"

class Mock_ClickEncoder : public ClickEncoder_interface
{
public:
    MOCK_METHOD(void, service, (), (override));
    MOCK_METHOD(int16_t, getValue, (), (override));
    MOCK_METHOD(ClickEncoder_interface::eButtonState, getButton, (), (override));

    MOCK_METHOD(void, setAccelerationEnabled, (const bool &enabled), (override));
    MOCK_METHOD(void, setDoubleClickEnabled, (const bool &enabled), (override));
};

#endif // CLICKENCODER_MOCKS_H