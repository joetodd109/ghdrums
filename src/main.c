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

#ifdef TESTING
static bool light;
#endif
#ifdef GHDRUMS
static uint8_t data[3];
static uint8_t buf[RX_BUFFER_SIZE];
static uint32_t head, headt;
static uint32_t tail, tailt;
static uint32_t timer[32];
#endif

int main(void) {
    SystemCoreClockUpdate();
    uint32_t i, n;
    uint8_t note, velocity;

    iox_led_init();
    timer_init();
    uart_init(31250);

    for (i = 0; i < RX_BUFFER_SIZE; i++) {
        buf[i] = 0;
    }
    for (i = 0; i < 32; i++) {
        timer[i] = 0;
    }
    n = 0;
    head = 0;
    tail = 0;
    headt = 0;
    tailt = 0;

#ifdef TESTING
    light = false;
#endif
    iox_led_on(true, true, false, false);

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
         * Fill data buffer to send ON command.
         */
        if (!uart_buffer_empty()) {
            data[n++] = uart_read_byte();
        }
        /* 
         * Send ON command, save data and time in circular 
         * buffer.
         */
        if (n == 3) {
            midi_on(0, data[1], data[2]);

            timer[headt] = timer_get() + 500000UL;
            headt = (headt + 1) % 32;

            for (i = 0; i < 3; i++) {
                buf[head] = data[i];
                head = (head + 1) % RX_BUFFER_SIZE;
            }
            n = 0;
        }
        /*
         * If time has passed, send OFF command. 
         */
        if (timer[tailt] > timer_get()) {
            /* ctrl byte not needed */
            tail = (tail + 1) % RX_BUFFER_SIZE;
            note = buf[tail];
            tail = (tail + 1) % RX_BUFFER_SIZE;
            velocity = buf[tail];
            tail = (tail + 1) % RX_BUFFER_SIZE;
            midi_off(0, note, velocity);
            tailt = (tailt + 1) % 32;
        }
#endif
    }
    return 0;
}


