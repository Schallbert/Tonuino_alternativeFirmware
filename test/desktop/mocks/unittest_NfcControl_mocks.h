#ifndef NFCCONTROL_MOCKS_H
#define NFCCONTROL_MOCKS_H

#include <gmock/gmock.h>

#include "../Nfc/NfcControl/NfcControl_interface.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;

// this object stores nfc tag data
class Mock_NfcControl : public NfcControl_interface
{
public:
    MOCK_METHOD(Nfc_interface::eTagState,  get_tag_presence, (), (override));
    MOCK_METHOD(bool, read_folder_from_card, (Folder &targetFolder), (override));
    MOCK_METHOD(bool, write_folder_to_card, (const Folder &sourceFolder), (override));
    MOCK_METHOD(bool, erase_card, (), (override));
#if DEBUGSERIAL
    MOCK_METHOD(void, print_debug_message, (), (override);
#endif
};

#endif //NFCCONTROL_MOCKS_H