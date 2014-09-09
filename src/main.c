#include "stm32f4xx.h"
#include <stdlib.h>

#include "utl.h"
#include "iox.h"
#include "uart.h"
#include "timer.h"
#include "midi.h"
#include "dma.h"     

//#define TESTING
#define GHDRUMS

#define TBUF_SIZE 32
#define BUF_SIZE 3072

static bool buffer_empty(void);

static bool light;
static uint8_t data[BUF_SIZE];
static uint32_t timer[TBUF_SIZE];
static uint32_t head, headt;
static uint32_t tail, tailt;
static uint32_t n;
static uart_recv_callback_fn callback;

int main(void) {
    SystemCoreClockUpdate();
    uint32_t i, tmp;
    callback = data_recv_callback;

    iox_led_init();
    timer_init();
    uart_init(31250, callback);
    dma_init();

    for (i = 0; i < TBUF_SIZE; i++) {
        timer[i] = 0;
    }
    head = 0;
    tail = 0;
    headt = 0;
    tailt = 0;
    n = 0;

    light = true;
    iox_led_on(false, false, false, light);

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
        if (n >= 3) {
            if (!buffer_empty()) {
                tmp = tail;
                midi_on(0, 
                        data[(tmp + 1) % BUF_SIZE], 
                        data[(tmp + 2) % BUF_SIZE]);
                timer[headt] = timer_get() + 500000UL;
                headt = (headt + 1) % TBUF_SIZE;
                light = !light;
                iox_led_on(false, false, false, light);
            }
            n = 0;
        }
        /*
         * If time has passed, send OFF command. 
         */
        if (timer[tailt] > timer_get()) {
            if (!buffer_empty()) {
                tmp = tail;
                midi_off(0, 
                        data[(tmp + 1) % BUF_SIZE], 
                        data[(tmp + 2) % BUF_SIZE]);
                tail = (tail + MIDI_CMD_LEN) % BUF_SIZE;
            }
            tailt = (tailt + 1) % TBUF_SIZE;
        }
#endif
    }
    return 0;
}

static bool 
buffer_empty(void)
{
    return (head == tail);
}

extern void
data_recv_callback(uint8_t bytes)
{
    data[head] = bytes;
    head = (head + 1) % BUF_SIZE;
    n++;
}
