#include "stm32f4xx.h"   
#include "stm32f4xx_conf.h" 
#include <stdlib.h>

#include "utl.h"
#include "iox.h"
#include "uart.h"
#include "timer.h"
#include "midi.h"       

//#define TESTING
#define GHDRUMS

#define TBUF_SIZE 32
#define BUF_SIZE 3072

#ifdef TESTING
static bool light;
#endif
#ifdef GHDRUMS
static uint8_t data[BUF_SIZE];
static uint32_t timer[TBUF_SIZE];
static uint32_t head, headt;
static uint32_t tail, tailt;
static uint32_t n;
static uart_recv_callback_fn callback;
#endif

int main(void) {
    SystemCoreClockUpdate();
    uint32_t i, tmp;
    callback = uart_recv_callback;

    iox_led_init();
    timer_init();
    uart_init(31250, callback);

    for (i = 0; i < TBUF_SIZE; i++) {
        timer[i] = 0;
    }
    head = 0;
    tail = 0;
    headt = 0;
    tailt = 0;
    n = 0;

#ifdef TESTING
    light = false;
#endif
    iox_led_on(true, false, false, true);

    while (1) {
#ifdef TESTING
        timer_delay(1000000UL); // wait 1s
        midi_on(1, n, 90);
        light = !light;
        n > 127 ? n = 0 : n;
        iox_led_on(true, light, false, false);
        timer_delay(500000UL); // wait 0.5s
        midi_off(1, n++, 90);
#endif
#ifdef GHDRUMS
        /* 
         * If full command received, send ON data and 
         * store timer.
         */
        if (n == 3) {
            tmp = tail;
            midi_on(0, data[tmp + 1], data[tmp + 2]);
            timer[headt] = timer_get() + 500000UL;
            headt = (headt + 1) % TBUF_SIZE;
            n = 0;
        }
        /*
         * If time has passed, send OFF command. 
         */
        if (timer[tailt] > timer_get()) {
            tmp = tail;
            midi_off(0, data[tmp + 1], data[tmp + 2]);
            tail = (tail + 3) % BUF_SIZE;
            tailt = (tailt + 1) % TBUF_SIZE;
        }
#endif
    }
    return 0;
}

extern void
uart_recv_callback(uint8_t byte)
{
    /* Fill data buffer */
    data[head] = byte;
    head = (head + 1) % BUF_SIZE;
    n++;
}
