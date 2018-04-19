#ifndef CORE_H
#define CORE_H

#include "core_global.h"

enum Replies{
    INVALID                     = -1,
    //settings replies
    SET_UP                      = 1,         //Successfully set up camera, bareer mode and number
    DVR_ERROR                   = -10,     //DVR Password or something else is incorrect

    //enter replies
    SNAPSHOT_FAIL               = -11,
    WIEGAND_REGISTERED          = 2,
    WIEGAND_ALREADY_REGISTERED,

    //exit replies
    WIEGAND_DEACTIVATED,
    WIEGAND_ALREADY_DEACTIVATED,
    WIEGAND_IS_MONTHLY,
    WIEGAND_NOT_REGISTERED      = -2,
};

class CORESHARED_EXPORT Core
{

public:
    Core();
};

#endif // CORE_H
