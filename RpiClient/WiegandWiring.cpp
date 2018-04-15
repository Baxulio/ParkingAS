#include "WiegandWiring.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>
#include <wiringPi.h>
void interrupt_d0(void){
    emit trigger.onTriggered_d0();
}
void interrupt_d1(void){
    emit trigger.onTriggered_d1();
}
void interrupt_timeout(int p){
    emit trigger.onTriggered_timeout(p);
}
#define WIEGANDMAXBITS 40

/* Set some timeouts */

/* Wiegand defines:
 * a Pulse Width time between 20 μs and 100 μs (microseconds)
 * a Pulse Interval time between 200 μs and 20000 μsec
 */

/* Each bit takes 4-6 usec, so all 26 bit sequence would take < 200usec */
#define WIEGAND_BIT_INTERVAL_TIMEOUT_USEC 20000 /* interval between bits, typically 1000us */

struct itimerval it_val;
struct sigaction sa;
static struct timespec wbit_tm; //for debug

void WiegandWiring::wiegand_sequence_reset()
{
    wds.bitcount = 0;
}

/* timeout handler, should fire after bit sequence has been read */
void WiegandWiring::reset_timeout_timer(long usec)
{
    it_val.it_value.tv_sec = 0;
    it_val.it_value.tv_usec = usec;

    it_val.it_interval.tv_sec = 0;
    it_val.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("setitimer");
        exit(1);
    }
}

int WiegandWiring::setup_wiegand_timeout_handler()
{
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = interrupt_timeout;
    //sa.sa_flags = SA_SIGINFO;

    if (options.debug)
        fprintf(stderr, "\n");

    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    };
    return 0;
}
/* Parse Wiegand 26bit format
 * Called wherever a new bit is read
 * bit: 0 or 1
 */
void WiegandWiring::add_bit_w26(int bit)
{
    static int parity0 = 0;
    static int parity1 = 0;

    //Parity calculation
    if (wds.bitcount == 0) {
        parity0 = 0;
        parity1 = 0;
    }
    if (wds.bitcount > 0 && wds.bitcount <= 12) {
        parity0 += bit;
    }
    else if (wds.bitcount >= 13 && wds.bitcount <= 24) {
        parity1 += bit;
    }
    //Code calculation
    if (wds.bitcount == 0) {
        /* Reset */
        wds.code_valid = 0;
        wds.facility_code = 0;
        wds.card_code = 0;
        wds.p0 = bit;           //parity
    }
    else if (wds.bitcount <= 8) {
        wds.facility_code <<= 1;
        if (bit)
            wds.facility_code |= 1;
    }
    else if (wds.bitcount < 25) {
        wds.card_code <<= 1;
        if (bit)
            wds.card_code |= 1;
    }
    else if (wds.bitcount == 25) {
        wds.p1 = bit;
        wds.full_code = wds.facility_code;
        wds.full_code = wds.full_code << 16;
        wds.full_code += wds.card_code;

        wds.code_valid = 1;
        //check parity
        if ((parity0 % 2) != wds.p0) {
            wds.code_valid = 0;
            if (options.debug) {
                fprintf(stderr, "Incorrect even parity bit (leftmost)\n");
            }
        }
        else if ((!(parity1 % 2)) != wds.p1) {
            wds.code_valid = 0;
            if (options.debug) {
                fprintf(stderr, "Incorrect odd parity bit (rightmost)\n");
            }
        }

    }
    else if (wds.bitcount > 25) {
        wds.code_valid = 0;
        wiegand_sequence_reset();

    }

    if (wds.bitcount < WIEGANDMAXBITS) {
        wds.bitcount++;
    }
}

unsigned long WiegandWiring::get_bit_timediff_ns()
{
    struct timespec now, delta;
    unsigned long tdiff;

    clock_gettime(CLOCK_MONOTONIC, &now);
    delta.tv_sec = now.tv_sec - wbit_tm.tv_sec;
    delta.tv_nsec = now.tv_nsec - wbit_tm.tv_nsec;

    tdiff = delta.tv_sec * 1000000000 + delta.tv_nsec;

    return tdiff;
}

WiegandWiring::WiegandWiring(QObject *parent, int debug) :
    QObject(parent)
{
    options.debug=debug;
    connect(&trigger, SIGNAL(onTriggered_d0()), this, SLOT(d0_pulse()));
    connect(&trigger, SIGNAL(onTriggered_d1()), this, SLOT(d1_pulse()));
    connect(&trigger, SIGNAL(onTriggered_timeout(int)), this, SLOT(wiegand_timeout(int)));
}

bool WiegandWiring::startWiegand(int d0pin, int d1pin, int bareerPin)
{
    /* defaults */
    options.d0pin = d0pin;
    options.d1pin = d1pin;
    options.bareerPin = bareerPin;

    if(setup_wiegand_timeout_handler()!=0)return false;

    if(wiringPiSetup()<0)return false;

    pinMode (bareerPin, OUTPUT) ;

    pinMode(d0pin, INPUT);
    pinMode(d1pin, INPUT);

    pullUpDnControl(d0pin, PUD_UP);
    pullUpDnControl(d1pin, PUD_UP);

    wiringPiISR(d0pin, INT_EDGE_FALLING, interrupt_d0);
    wiringPiISR(d1pin, INT_EDGE_FALLING, interrupt_d1);

    wiegand_sequence_reset();
    return true;
}

void WiegandWiring::openBareer()
{
    digitalWrite (options.bareerPin, HIGH); // On
    delay (1000) ;		// mS
    digitalWrite (options.bareerPin, LOW) ; // Off
}

/* Timeout from last bit read, sequence may be completed or stopped */
void WiegandWiring::wiegand_timeout(int p)
{
    if (options.debug)
        fprintf(stderr, "wiegand_timeout()\n");
    wiegand_sequence_reset();

    //show_code();
    emit onReadyRead(wds.full_code);
}

void WiegandWiring::d0_pulse()
{
    reset_timeout_timer(WIEGAND_BIT_INTERVAL_TIMEOUT_USEC);     //timeout waiting for next bit
    if (options.debug) {
        fprintf(stderr, "Bit:%02ld, Pulse 0, %ld us since last bit\n",
                wds.bitcount, get_bit_timediff_ns() / 1000);
        clock_gettime(CLOCK_MONOTONIC, &wbit_tm);
    }
    add_bit_w26(0);

}

void WiegandWiring::d1_pulse()
{
    reset_timeout_timer(WIEGAND_BIT_INTERVAL_TIMEOUT_USEC);     //timeout waiting for next bit
    if (options.debug) {
        fprintf(stderr, "Bit:%02ld, Pulse 1, %ld us since last bit\n",
                wds.bitcount, get_bit_timediff_ns() / 1000);
        clock_gettime(CLOCK_MONOTONIC, &wbit_tm);
    }
    add_bit_w26(1);
}

void WiegandWiring::show_code()
{
    printf("\n");
    printf("*** Code Valid: %d\n", wds.code_valid);
    printf("*** Facility code: %d(dec) 0x%X\n", wds.facility_code,
           wds.facility_code);
    printf("*** Card code: %d(dec) 0x%X\n", wds.card_code, wds.card_code);
    printf("*** Full code: %d\n", wds.full_code);
    printf("*** Parity 0:%d Parity 1:%d\n", wds.p0, wds.p1);
    fflush(stdout);
}
