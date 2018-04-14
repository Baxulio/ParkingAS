#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

///////////////////////////////////
//// WIEGAND
///

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define D0_PIN 29
#define D1_PIN 28

#define MAX_BITS 100                 // max number of bits
#define WIEGAND_WAIT_TIME 300000      // time to wait for another wiegand pulse.

static unsigned char databits[MAX_BITS];    // stores all of the data bits
static unsigned char bitCount;              // number of bits currently captured
static unsigned int flagDone;              // goes low when data is currently being captured
static unsigned int wiegand_counter;        // countdown until we assume there are no more bits

static unsigned long facilityCode=0;        // decoded facility code
static unsigned long cardCode=0;            // decoded card code

class Trigger : public QObject
{
    Q_OBJECT
signals:
    void onTriggered_ShowCode(quint32 value);
    void onTriggered_timeout();
    void onTriggered_UnknownFormat(const QString &str);
};

static Trigger trigger;

PI_THREAD (waitForData0)
{

    (void)piHiPri (10) ;	// Set this thread to be high priority

    for (;;)
    {
        if (waitForInterrupt (D0_PIN, -1) > 0)	// Got it
        {
            //printf ("0") ; fflush (stdout) ;
            bitCount++;
            flagDone = 0;
            wiegand_counter = WIEGAND_WAIT_TIME;
        }

    }
}

PI_THREAD (waitForData1)
{

    (void)piHiPri (10) ;	// Set this thread to be high priority

    for (;;)
    {
        if (waitForInterrupt (D1_PIN, -1) > 0)	// Got it
        {
            //printf ("1") ; fflush (stdout) ;
            databits[bitCount] = 1;
            bitCount++;
            flagDone = 0;
            wiegand_counter = WIEGAND_WAIT_TIME;
        }
    }
}

bool setup(void)
{
    system ("gpio edge 0 falling") ;
    system ("gpio edge 1 falling") ;

    // Setup wiringPi

    if(wiringPiSetupSys ()<0)return false;

    // Fire off our interrupt handler

    piThreadCreate (waitForData0) ;
    piThreadCreate (waitForData1) ;

    wiegand_counter = WIEGAND_WAIT_TIME;
    return true;
}

void printBits()
{
    // Prints out the results
    printf ("Read %d bits\n", bitCount) ; fflush (stdout) ;
    printf ("Facility Code: %d\n", facilityCode) ; fflush (stdout) ;
    printf ("TagID: %d\n\n", cardCode) ; fflush (stdout) ;
    return;
}

////////// LOOOOOOOOOOOP
///
void loooooop(const char &siteCode){
    char useSiteCode = siteCode; // determines whether or no to use 26 bit site code.
    unsigned char i;

    // This waits to make sure that there have been no more data pulses before processing data
    if (!flagDone) {
        if (--wiegand_counter == 0)
            flagDone = 1;
    }

    // if we have bits and the wiegand counter reached 0
    if (bitCount > 0 && flagDone) {

        //Full wiegand 26 bit
        if (bitCount == 26 & useSiteCode == 'N')
        {
            //unsigned char i;

            // card code = bits 2 to 25
            for (i=1; i<25; i++)
            {
                cardCode <<=1;
                cardCode |= databits[i];
            }

            //printBits();
            emit trigger.onTriggered_ShowCode(cardCode);
        }

        else if (bitCount == 26 & useSiteCode == 'Y')
        {
            // standard 26 bit format with site code
            // facility code = bits 2 to 9
            for (i=1; i<9; i++)
            {
                facilityCode <<=1;
                facilityCode |= databits[i];
            }

            // card code = bits 10 to 23
            for (i=9; i<25; i++)
            {
                cardCode <<=1;
                cardCode |= databits[i];
            }

            //printBits();
            emit trigger.onTriggered_ShowCode(cardCode);
        }

        // 35 bit HID Corporate 1000 format
        else if (bitCount == 35)
        {

            // facility code = bits 2 to 14
            for (i=2; i<14; i++)
            {
                facilityCode <<=1;
                facilityCode |= databits[i];
            }

            // card code = bits 15 to 34
            for (i=14; i<34; i++)
            {
                cardCode <<=1;
                cardCode |= databits[i];
            }

            //printBits();
            emit trigger.onTriggered_ShowCode(cardCode);
        }
        //HID Proprietary 37 Bit Format with Facility Code: H10304
        else if (bitCount == 37)
        {

            // facility code = bits 2 to 17
            for (i=2; i<17; i++)
            {
                facilityCode <<=1;
                facilityCode |= databits[i];
            }

            // card code = bits 18 to 36
            for (i=18; i<36; i++)
            {
                cardCode <<=1;
                cardCode |= databits[i];
            }

            //printBits();
            emit trigger.onTriggered_ShowCode(cardCode);
        }
        else {
            emit trigger.onTriggered_UnknownFormat("<font color='red'>Unknown format of Wiegand!")
                    // Other formats to be added later.
                    //printf ("Unknown format\n") ; fflush (stdout) ;
                    //printf ("Wiegand Counter = %d\n", wiegand_counter) ; fflush (stdout) ;
                    //printf ("Flag done = %d\n", flagDone) ; fflush (stdout) ;
        }
        // cleanup and get ready for the next card
        bitCount = 0;
        facilityCode = 0;
        cardCode = 0;
        for (i=0; i<MAX_BITS; i++)
        {
            databits[i] = 0;
        }
        emit trigger.onTriggered_timeout();
    }
}
//////////END LOOOOOOOOOOOOOOOOP//////


// END WIEGAND///////////////
/////////////////////////////

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(rpiclientresources);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Baxulio");
    QCoreApplication::setApplicationName("ParkingAS");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    if(!setup()){
        return EXIT_FAILURE;
    }
    MainWindow w;
    w.show();

    QObject::connect(&w,SIGNAL(connected(bool)),[](bool b){
        b?QObject::connect(&trigger, SIGNAL(onTriggered_timeout()),loooooop('N')):
          QObject::disconnect(&trigger, SIGNAL(onTriggered_timeout()));
    });

            QObject::connect(&trigger, SIGNAL(onTriggered_ShowCode(quint32)),&w, SLOT(wiegandCallback(quint32)));
    QObject::connect(&trigger, SIGNAL(onTriggered_UnknownFormat(QString)),&w,SLOT(showStatusMessage(QString)));

    return app.exec();
}
