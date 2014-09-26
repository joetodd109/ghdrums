/**
 ******************************************************************************
 * @file    uart.c
 * @author  Joe Todd
 * @version
 * @date    August 2014
 * @brief   MIDI Interface
 *
  ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "uart.h"
#include "utl.h"
#include "iox.h"

#define USART2_TX_PIN   2   /* PA2 */
#define USART1_RX_PIN   7   /* PB7 */

#define USART1_AF       7   /* AF7 */

/* Private typedefs -----------------------------------------------------------*/

/* Private variables ----------------------------------------------------------*/
static uint32_t uart_recv_cnt;
static uart_recv_callback_fn callback;

/* Function Declarations ------------------------------------------------------*/
extern void 
uart_init(uint32_t baudrate, uart_recv_callback_fn callback_fn)
{
    uint32_t reg;
    uint32_t fraction;
    uint32_t mantissa;
    callback = callback_fn;

    uart_recv_cnt = 0;

    /* Enable GPIO clock */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN
        | RCC_AHB1ENR_GPIOAEN);

    /* Enable UART clock */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* Connect PXx to USARTx_Tx/Rx*/
    iox_alternate_func(iox_port_a, USART2_TX_PIN, USART1_AF);
    iox_alternate_func(iox_port_b, USART1_RX_PIN, USART1_AF);

    /* Configure USART Tx/Rx as alternate function  */
    iox_configure_pin(iox_port_a, USART2_TX_PIN, iox_mode_af, iox_type_pp, 
                        iox_speed_fast, iox_pupd_up);
    iox_configure_pin(iox_port_b, USART1_RX_PIN, iox_mode_af, iox_type_pp,
                        iox_speed_fast, iox_pupd_up);

    /* 1 Stop bit, asynchronous mode */
    USART1->CR2 = 0x00;
    USART2->CR2 = 0x00; 
    /* Rx interrupt enabled, Tx/Rx enabled */
    USART1->CR1 = (USART_CR1_RXNEIE | USART_CR1_RE);
    USART2->CR1 = USART_CR1_TE;
    /* No hardware flow control, DMA enabled */
    USART1->CR3 = 0x00;
    USART2->CR3 = 0x00;
    /* Configure baudrate */
    mantissa = ((25 * PCLK2) / (4 * baudrate));
    reg = (mantissa / 100) << 4;
    fraction = mantissa - (100 * (reg >> 4));
    reg |= (((fraction * 16) + 50) / 100) & 0x0F;
    USART1->BRR = reg;
    USART2->BRR = reg;

    utl_enable_irq(USART1_IRQn);

    USART1->CR1 |= USART_CR1_UE;
    USART2->CR1 |= USART_CR1_UE;
}

extern void 
uart_send_data(unsigned char *buf, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        USART2->DR = buf[i];
        while ((USART2->SR & USART_SR_TC) == 0);
    }
}

void USART1_IRQHandler(void)
{
    uint32_t sr;

    sr = USART1->SR;

    if (sr & USART_SR_RXNE) {
        callback(USART1->DR);

        USART1->SR &= ~USART_SR_RXNE;
        uart_recv_cnt++;
    }
}