#include "stm32f4xx.h"
#include <stdlib.h>

#include "utl.h"
#include "iox.h"
#include "uart.h"
#include "timer.h"
#include "midi.h"    

//#define TESTING
#define GHDRUMS

#define TBUF_SIZE 32
#define BUF_SIZE 300

static bool buffer_empty(void);
static bool tbuffer_empty(void);

static bool light;
static uint8_t data[BUF_SIZE];
static uint32_t timer[TBUF_SIZE];
static uint32_t head, headt;
static uint32_t tail, tailt;
static uint32_t n;
static uart_recv_callback_fn callback;

int main(void) {
    SystemCoreClockUpdate();
    uint32_t i;
    callback = data_recv_callback;

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

    light = true;
    iox_led_on(false, light, false, false);

    while (1) {
#ifdef TESTING
        timer_delay(10); // wait 1s
        midi_on(1, n, 90);
        light = !light;
        n > 127 ? n = 0 : n;
        iox_led_on(true, light, false, false);
        timer_delay(5); // wait 0.5s
        midi_off(1, n++, 90);
#endif
#ifdef GHDRUMS
        /* 
         * If full command received, send ON data and 
         * store timer.
         */
        if (n >= 3) {
            if (!buffer_empty()) {
                midi_on(0, 
                        data[(tail + 1)], 
                        data[(tail + 2)]);
                timer[headt++] = timer_get() + 5;
                headt %= TBUF_SIZE;
                light = !light;
                iox_led_on(false, false, false, light);
                n -= 3;
            }
        }
        /*
         * If time has passed, send OFF command. 
         */
        if (!tbuffer_empty()) {
            if (timer[tailt] > timer_get()) {
                if (!buffer_empty()) {
                    midi_off(0, 
                            data[(tail + 1)], 
                            data[(tail + 2)]);
                    tail = (tail + MIDI_CMD_LEN) % BUF_SIZE;
                    tailt = (tailt + 1) % TBUF_SIZE;
                }
            }
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

static bool
tbuffer_empty(void)
{
    return (headt == tailt);
}

extern void
data_recv_callback(uint8_t bytes)
{
    data[head++] = bytes;
    head %= BUF_SIZE;
    n++;
}
