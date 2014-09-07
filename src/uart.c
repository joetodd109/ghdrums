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
#include "dma.h"

#define USART1_TX_PIN   6   /* PB6 */
#define USART1_RX_PIN   7   /* PB7 */

#define USART1_AF       7   /* AF7 */

#define MIDI_CMD_LEN    3

/* Private typedefs -----------------------------------------------------------*/

/* Private variables ----------------------------------------------------------*/
static uint8_t data[MIDI_CMD_LEN];
static uint32_t uart_recv_cnt;
static uint32_t uart_dma_tc_cnt;
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
    uart_dma_tc_cnt = 0;

    /* Enable GPIO clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* Enable UART clock */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* Connect PXx to USARTx_Tx/Rx*/
    iox_alternate_func(iox_port_b, USART1_TX_PIN, USART1_AF);
    iox_alternate_func(iox_port_b, USART1_RX_PIN, USART1_AF);

    /* Configure USART Tx/Rx as alternate function  */
    iox_configure_pin(iox_port_b, USART1_TX_PIN, iox_mode_af, iox_type_pp, 
                        iox_speed_fast, iox_pupd_up);
    iox_configure_pin(iox_port_b, USART1_RX_PIN, iox_mode_af, iox_type_pp,
                        iox_speed_fast, iox_pupd_up);

    /* 1 Stop bit, asynchronous mode */
    USART1->CR2 = 0x00; 
    /* Rx interrupt enabled, Tx/Rx enabled */
    USART1->CR1 = (USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE);
    /* No hardware flow control, DMA enabled */
    USART1->CR3 = USART_CR3_DMAR;
    /* Configure baudrate */
    mantissa = ((25 * PCLK2) / (4 * baudrate));
    reg = (mantissa / 100) << 4;
    fraction = mantissa - (100 * (reg >> 4));
    reg |= (((fraction * 16) + 50) / 100) & 0x0F;
    USART1->BRR = reg;

    utl_enable_irq(USART1_IRQn);
    uart_configure_dma2(data, MIDI_CMD_LEN);

    USART1->CR1 |= USART_CR1_UE;
}

extern void 
uart_send_data(unsigned char *buf, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        USART1->DR = buf[i];
        while ((USART1->SR & USART_SR_TC) == 0);
    }
}

extern void
uart_configure_dma2(uint8_t * const src, uint16_t nbytes)
{
    uint8_t dma_stream;
    dma_stream = 2u;                   /* USART1_Rx = DMA2_Stream2 */

    DMA_Stream_TypeDef cfg = {
        .CR = (1u << DMA_CR_TCIE_Pos)  /* Transfer complete interrupt enabled. */
            |(0u << DMA_CR_HTIE_Pos)   /* No half-transfer interrupt. */
            |(0u << DMA_CR_TEIE_Pos)   /* Transfer error interrupt disabled. */
            |(0u << DMA_CR_DIR_Pos)    /* Read from peripheral to memory. */
            |(1u << DMA_CR_CIRC_Pos)   /* Circular mode. */
            |(0u << DMA_CR_PINC_Pos)   /* Don't increment peripheral address. */
            |(1u << DMA_CR_MINC_Pos)   /* Do increment memory address. */
            |(0u << DMA_CR_PSIZE_Pos)  /* 8-bit peripheral size. */
            |(0u << DMA_CR_PFCTRL_Pos) /* DMA flow controller */
            |(0u << DMA_CR_MSIZE_Pos)  /* 8-bit memory size. */
            |(2u << DMA_CR_PL_Pos)     /* High priority. */
            |(4u << DMA_CR_CHSEL_Pos), /* Channel Selection. */
        .NDTR = nbytes,
        .PAR = (uint32_t) & USART1->DR,
        .M0AR = (uint32_t) src,
    };

    dma_init_dma2_chx(dma_stream, (DMA_Stream_TypeDef const *) &cfg);
}

void DMA2_Stream2_IRQHandler(void) {
    uint32_t isr;

    isr = DMA2->LISR;

    /* clear status reg */
    DMA2->LIFCR = isr;

    if (isr & DMA_LISR_TCIF2) {
        callback(data);

        uart_dma_tc_cnt++;    
    }
}

void USART1_IRQHandler(void)
{
    //uint8_t data;
    uint32_t sr;

    sr = USART1->SR;

    if (sr & USART_SR_RXNE) {
        //data = USART1->DR;
        //callback(data);

        USART1->SR &= ~USART_SR_RXNE;
        uart_recv_cnt++;
    }
}