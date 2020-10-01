#ifndef UNITTEST_ARDUINODICONTAINER_MOCKS_H
#define UNITTEST_ARDUINODICONTAINER_MOCKS_H

#include <gmock/gmock.h>
#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"
#include "../Arduino/Arduino_interface/Arduino_interface.h"

using ::testing::NiceMock;

class Fake_ArduinoDIcontainer : public Arduino_DIcontainer_interface
{
public:
    Arduino_interface_pins *getPins() override;
    Arduino_interface_delay *getDelay() override;
    Arduino_interface_com *getSerial() override;
    Arduino_interface_random *getRandom() override;
    Arduino_interface_eeprom *getEeprom() override;

private:
    NiceMock<Mock_pinCtrl> m_pins;
    NiceMock<Mock_delay> m_delay;
    NiceMock<Mock_com> m_serial;
    NiceMock<Mock_random> m_random;
    NiceMock<Mock_eeprom> m_eeprom;
}

class Mock_ArduinoDIcontainer : public Arduino_DIcontainer_interface
{
public:
    MOCK_METHOD(Arduino_interface_pins *, getPins, (), (override));
    MOCK_METHOD(Arduino_interface_delay *, getDelay, (), (override));
    MOCK_METHOD(Arduino_interface_com *, getSerial, (), (override));
    MOCK_METHOD(Arduino_interface_random *, getRandom, (), (override));
    MOCK_METHOD(Arduino_interface_eeprom *, getEeprom, (), (override));

    // "Tunnels" DIcontainer.
    // Returns a mock instance of any of the corresponding parent methods.
    void DelegateToFake()
    {
        ON_CALL(*this, getPins).WillByDefault({ return m_Fake.getPins(); });
        ON_CALL(*this, getDelay).WillByDefault({ return m_Fake.getDelay(); });
        ON_CALL(*this, getSerial).WillByDefault({ return m_Fake.getSerial(); });
        ON_CALL(*this, getRandom).WillByDefault({ return m_Fake.getRandom(); });
        ON_CALL(*this, geEeprom).WillByDefault({ return m_Fake.getEeprom(); });
    }

private:
    Fake_ArduinoDIcontainer m_fake{};
};
#endif // UNITTEST_ARDUINODICONTAINER_MOCKS_H