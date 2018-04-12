#ifndef WIEGANDWIRING_H
#define WIEGANDWIRING_H

#include <QObject>

struct wiegand_data {
    unsigned char p0, p1;       //parity 0 , parity 1
    quint8 facility_code;
    quint16 card_code;
    quint32 full_code;
    int code_valid;
    unsigned long bitcount;     // bits read
};
struct option_s {
    int d0pin;
    int d1pin;
    int bareerPin;
    int debug;
};

class WiegandWiring : public QObject
{
    Q_OBJECT
private:
    wiegand_data wds;
    option_s options;

    void wiegand_sequence_reset();
    void reset_timeout_timer(long usec);
    int setup_wiegand_timeout_handler();

    void add_bit_w26(int bit);
    unsigned long get_bit_timediff_ns();

public:
    explicit WiegandWiring(QObject *parent = nullptr, int debug = 0);

    bool startWiegand(int d0pin = 29, int d1pin = 28, int bareerPin = 0);
    void cancel(void);
    bool openBareer();
    void show_code();

signals:
    void onReadyRead(quint32 full_code);

private slots:
    void wiegand_timeout();
    void d0_pulse(void);
    void d1_pulse(void);
};

#endif // WIEGANDWIRING_H
