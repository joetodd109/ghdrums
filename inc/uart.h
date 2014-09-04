/**
  ******************************************************************************
  * @file    uart.h 
  * @author  Joe Todd
  * @version 
  * @date    
  * @brief   Header for uart.c
  *
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UART_H
#define UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
#include "iox.h"

/* Definitions ---------------------------------------------------------------*/
#define RX_BUFFER_SIZE 4096

typedef void (*uart_recv_callback_fn) (uint8_t data);

extern void uart_init(uint32_t baudrate, uart_recv_callback_fn callback);
extern void uart_send_data(unsigned char *buf, uint32_t len);
extern bool uart_buffer_empty(void);
extern uint8_t uart_read_byte(void);
extern uint8_t uart_get_byte(uint32_t i);
extern void uart_recv_callback(uint8_t data);

#endif