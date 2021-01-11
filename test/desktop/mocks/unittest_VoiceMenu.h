#ifndef UNITTEST_VOICEMENU_H
#define UNITTEST_VOICEMENU_H

#include <gtest/gtest.h>

MATCHER(invalidPrompt, "")
{
    return (arg.getId() == 0);
}

MATCHER_P(identicalPrompt, comp, "")
{
    return (arg == comp);
}

MATCHER(invalidFolder, "")
{
    return (!arg.isInitiated());
}

MATCHER(validFolder, "")
{
    return (arg.isInitiated());
}

MATCHER_P(identicalFolder, comp, "")
{
    return ((arg.getFolderId() == comp.getFolderId()) &&
            (arg.getPlayMode() == comp.getPlayMode()) &&
            (arg.getTrackCount() == comp.getTrackCount()));
}

#endif // UNITTEST_VOICEMENU_H