#include <pigpio.h>
#include "wiegand.hpp"

bool Wiegand::startWiegand(int gpio_0, int gpio_1, int timeout)
{
    if(gpioInitialise()<0)return false;
    /*
       Instantiate with the gpio for 0 (green wire), the gpio for 1
       (white wire), the callback function, and the bit timeout in
       milliseconds which indicates the end of a code.

       The callback is passed the code length in bits and the value.
    */

    mygpio_0 = gpio_0;
    mygpio_1 = gpio_1;

    mytimeout = timeout;

    in_code = 0;

    gpioSetMode(gpio_0, PI_INPUT);
    gpioSetMode(gpio_1, PI_INPUT);

    gpioSetPullUpDown(gpio_0, PI_PUD_UP);
    gpioSetPullUpDown(gpio_1, PI_PUD_UP);

    gpioSetAlertFuncEx(gpio_0, _cbEx, this);
    gpioSetAlertFuncEx(gpio_1, _cbEx, this);

    return true;
}

void Wiegand::_cb(int gpio, int level, quint32 tick)
{
    /*
      Accumulate bits until both gpios 0 and 1 timeout.
   */

    if (level == 0) /* a falling edge indicates a new bit */
    {
        if (!in_code)
        {
            bits = 1;
            num = 0;

            in_code = 1;
            code_timeout = 0;

            gpioSetWatchdog(mygpio_0, mytimeout);
            gpioSetWatchdog(mygpio_1, mytimeout);
        }
        else
        {
            bits++;
            num <<= 1;
        }

        if (gpio == mygpio_0)
        {
            code_timeout &= 2; /* clear gpio 0 timeout */
        }
        else
        {
            code_timeout &= 1; /* clear gpio 1 timeout */
            num |= 1;
        }
    }
    else if (level == PI_TIMEOUT)
    {
        if (in_code)
        {
            if (gpio == mygpio_0)
            {
                code_timeout |= 1; /* timeout gpio 0 */
            }
            else
            {
                code_timeout |= 2; /* timeout gpio 1 */
            }

            if (code_timeout == 3) /* both gpios timed out */
            {
                gpioSetWatchdog(mygpio_0, 0);
                gpioSetWatchdog(mygpio_1, 0);

                in_code = 0;

                emit onReadyRead(bits,num);
            }
        }
    }
}

void Wiegand::_cbEx(int gpio, int level, quint32 tick, void *user)
{
    /*
      Need a static callback to link with C.
   */

    Wiegand *mySelf = (Wiegand *) user;

    mySelf->_cb(gpio, level, tick); /* Call the instance callback. */
}

Wiegand::Wiegand(QObject *parent):
    QObject(parent)
{

}

void Wiegand::cancel(void)
{
    /*
      Cancel the Wiegand decoder.
   */
    gpioSetAlertFuncEx(mygpio_0, 0, this);
    gpioSetAlertFuncEx(mygpio_1, 0, this);

    gpioTerminate();
}

bool Wiegand::openBareer()
{
    if(gpioWrite(24,1)<0)
        return false;
    sleep(0.5);
    gpioWrite(24,0);
    return true;
    //Here goes pigpio bareer controll
}

