#ifndef WIEGANDWIRING_H
#define WIEGANDWIRING_H

#include <QObject>
#include <wiringPi.h>

class Trigger
{
    Q_OBJECT
signals:
    void trigger_show_code(quint32 code);
};

static Trigger trigger;

/* Defaults, change with command-line options */

#define DEBUG_MODE 0

struct wiegand_data {
    unsigned char p0, p1;       //parity 0 , parity 1
    quint8 facility_code;
    quint16 card_code;
    quint32 full_code;
    int code_valid;
    unsigned long bitcount;     // bits read
}wds;
struct option_s {
    int d0pin;
    int d1pin;
    int bareerPin;
    int debug;
}options;

/* Timeout from last bit read, sequence may be completed or stopped */
void wiegand_timeout(int u);
void show_code();
int setup_wiegand_timeout_handler();
/* Parse Wiegand 26bit format
 * Called wherever a new bit is read
 * bit: 0 or 1
 */
void add_bit_w26(int bit);
unsigned long get_bit_timediff_ns();
void d0_pulse(void);
void d1_pulse(void);
void wiegand_sequence_reset();
/* timeout handler, should fire after bit sequence has been read */
void reset_timeout_timer(long usec);
void openBareer();
bool gpio_init(int d0pin, int d1pin, int bPin);

#endif // WIEGANDWIRING_H
