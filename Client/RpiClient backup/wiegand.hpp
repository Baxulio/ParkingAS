#ifndef WIEGAND_HPP
#define WIEGAND_HPP

#include <QObject>

class Wiegand : public QObject
{
    Q_OBJECT

    void _cb(int gpio, int level, quint32 tick);
    /* Need a static callback to link with C. */
    static void _cbEx(int gpio, int level, quint32 tick, void *user);

public:

    Wiegand(QObject *parent = 0);
    /*
      This function establishes a Wiegand decoder on gpio_0 and gpio_1.

      A gap of timeout milliseconds without a new bit indicates
      the end of a code.

      When the code is ended the callback function is called with the code
      bit length and value.
   */
    bool startWiegand(int gpio_0, int gpio_1, int timeout=5);
    void cancel(void);
    /*
      This function releases the resources used by the decoder.
   */
signals:
    void onReadyRead(int bits, quint32 value);

private:
    int mygpio_0, mygpio_1, mytimeout, in_code, bits;
    quint32 num;
    quint32 code_timeout;
};

#endif

