/**
 ******************************************************************************
 * @file    midi.c
 * @author  Joe Todd
 * @version
 * @date    August 2014
 * @brief   MIDI Interface
 *
  ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "midi.h"
#include "uart.h"

/* Private typedefs -----------------------------------------------------------*/

/* Private variables ----------------------------------------------------------*/


/* Function Definitions -------------------------------------------------------*/

extern bool
midi_on(uint8_t ch, uint8_t note, uint8_t velocity)
{
    uint8_t buf[MIDI_CMD_LEN];

    if ((ch > 16) || (note > 127) || (velocity > 127)) {
        return false;
    }

    buf[0] = (MIDI_NOTE_ON & 0xF0) | (ch & 0x0F);
    buf[1] = note;
    buf[2] = velocity;

    uart_send_data(buf, MIDI_CMD_LEN);

    return true;
}

extern bool
midi_off(uint8_t ch, uint8_t note, uint8_t velocity)
{
    uint8_t buf[MIDI_CMD_LEN];

    if ((ch > 16) || (note > 127) || (velocity > 127)) {
        return false;
    }

    buf[0] = (MIDI_NOTE_OFF & 0xF0) | (ch & 0x0F);
    buf[1] = note;
    buf[2] = velocity;

    uart_send_data(buf, MIDI_CMD_LEN);

    return true;
}

extern bool
midi_poly_aftertouch(uint8_t ch, uint8_t note, uint8_t pressure)
{
    uint8_t buf[MIDI_CMD_LEN];

    if ((ch > 16) || (note > 127) || (pressure > 127)) {
        return false;
    }

    buf[0] = (MIDI_POLY_AFTERTOUCH & 0xF0) | (ch & 0x0F);
    buf[1] = note;
    buf[2] = pressure;

    uart_send_data(buf, MIDI_CMD_LEN);

    return true;
}

extern bool
midi_ctrl_mode_change(uint8_t ch, uint8_t func, uint8_t value)
{
    uint8_t buf[MIDI_CMD_LEN];

    if ((ch > 16) || (func > 127) || (value > 127)) {
        return false;
    }

    buf[0] = (MIDI_CTRL_MODE_CHANGE & 0xF0) | (ch & 0x0F);
    buf[1] = func;
    buf[2] = value;

    uart_send_data(buf, MIDI_CMD_LEN);

    return true;
}

extern bool
midi_prog_change(uint8_t ch, uint8_t prog)
{
    uint8_t buf[MIDI_CMD_LEN];

    if ((ch > 16) || (prog > 127)) {
        return false;
    }

    buf[0] = (MIDI_PROG_CHANGE & 0xF0) | (ch & 0x0F);
    buf[1] = prog;

    uart_send_data(buf, 2);

    return true;
}

extern bool
midi_chan_aftertouch(uint8_t ch, uint8_t pressure)
{
    uint8_t buf[MIDI_CMD_LEN];

    if ((ch > 16) || (pressure > 127)) {
        return false;
    }

    buf[0] = (MIDI_CHAN_AFTERTOUCH & 0xF0) | (ch & 0x0F);
    buf[1] = pressure;

    uart_send_data(buf, 2);

    return true;
}

extern bool
midi_pitch_bend(uint8_t ch, uint8_t lsb, uint8_t msb)
{
    uint8_t buf[MIDI_CMD_LEN];

    if ((ch > 16) || (lsb > 127) || (msb > 127)) {
        return false;
    }

    buf[0] = (MIDI_PITCH_BEND & 0xF0) | (ch & 0x0F);
    buf[1] = lsb;
    buf[2] = msb;

    uart_send_data(buf, MIDI_CMD_LEN);

    return true;
}