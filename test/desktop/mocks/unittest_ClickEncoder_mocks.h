#ifndef CLICKENCODER_MOCKS_H
#define CLICKENCODER_MOCKS_H

#include <gmock/gmock.h>

#include "../UserInput/ClickEncoder_interface/ClickEncoder_interface.h"

class Mock_ClickEncoder : public ClickEncoder_interface
{
    MOCK_METHOD(void, service, () , (override));
    MOCK_METHOD(int16_t, get_value, (), (override));
    MOCK_METHOD(ClickEncoder_interface::eButtonState, get_button, (), (override));

    MOCK_METHOD(void, setAccelerationEnabled, (const bool &enabled), (override));
    MOCK_METHOD(void, setDoubleclickEnabled, (const bool &enabled), (override));
};

#endif // CLICKENCODER_MOCKS_H