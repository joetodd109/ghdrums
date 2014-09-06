/**
  ******************************************************************************
  * @file    midi.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for midi.c
  *
  ******************************************************************************
*/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MIDI_H
#define MIDI_H

/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"
#include "stm32f4xx.h"

/* Global Defines ----------------------------------------------------------- */

#define MIDI_CMD_LEN    3u

/* MIDI Commands ---------- */
#define MIDI_NOTE_OFF   128u
#define MIDI_NOTE_ON    144u
#define MIDI_POLY_AFTERTOUCH    160u
#define MIDI_CTRL_MODE_CHANGE   176u
#define MIDI_PROG_CHANGE        192u
#define MIDI_CHAN_AFTERTOUCH    208u
#define MIDI_PITCH_BEND         224u

/* Control/Mode Changes --- */
#define CTRL_BANK_SELECT        0u
#define CTRL_MOD_WHEEL          1u 
#define CTRL_BREATH_CTRL        2u
#define CTRL_FOOT_CTRL          4u
#define CTRL_PORTAMENTO_T       5u
#define CTRL_DATA_ENTRY         6u
#define CTRL_CH_VOLUME          7u
#define CTRL_BALANCE            8u
#define CTRL_PAN                10u
#define CTRL_EXPR_CTRL          11u
#define CTRL_EFFECT_CTRL1       12u
#define CTRL_EFFECT_CTRL2       13u
#define CTRL_GP_CTRL1           16u
#define CTRL_GP_CTRL2           17u
#define CTRL_GP_CTRL3           18u
#define CTRL_GP_CTRL4           19u    

extern bool midi_on(uint8_t ch, uint8_t note, uint8_t velocity);
extern bool midi_off(uint8_t ch, uint8_t note, uint8_t velocity);
extern bool midi_poly_aftertouch(uint8_t ch, uint8_t note, uint8_t pressure);
extern bool midi_ctrl_mode_change(uint8_t ch, uint8_t func, uint8_t value);
extern bool midi_prog_change(uint8_t ch, uint8_t prog);
extern bool midi_chan_aftertouch(uint8_t ch, uint8_t pressure);
extern bool midi_pitch_bend(uint8_t ch, uint8_t lsb, uint8_t msb);

#endif