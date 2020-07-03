#include <gtest/gtest.h>
#include <gmock/gmock.h>
//#include <Folder.h>
//#include "../src/MFRC522_Interface/MFRC522_interface.h"
#include "../src/NfcTag/NfcTag.h"
#include "mocks/unittest_Folder_mocks.h"
#include "mocks/unittest_NfcTag_mocks.h"

using ::testing::Return; 
using ::testing::AtLeast;   

TEST(NfcTag, basicTest)
{
    EXPECT_EQ(4, 2+2);
}


TEST(NfcTag_init, IsCalledOnConstruction)
{
    Mock_MFRC522 mfrc;
    EXPECT_CALL(mfrc, initReader()).Times(AtLeast(1));
    NfcTag nfc{NfcTag(&mfrc)};
    //NfcTag nfc(&mfrc);
}

/*
TEST(NfcTag_Write, invalidFolder_ReturnsFalse)
{
    Folder testFolder;
    Mock_MFRC522 mockMfrc;
    NfcTag nfc(&mockMfrc);
    EXPECT_TRUE(true);
    //EXPECT_FALSE(nfc.write_folder_to_card(testFolder));
}
*/