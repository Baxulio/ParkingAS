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

#define D0_PIN 29
#define D1_PIN 28
#define BAREER_PIN 24
#define WIEGANDMAXBITS 35
#define WIEGAND_BIT_INTERVAL_TIMEOUT_USEC 20000

class CORESHARED_EXPORT Core
{

public:
    Core();
};

#endif // CORE_H
