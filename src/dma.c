/**
 ******************************************************************************
 * @file    dma.c
 * @author  Joe Todd
 * @version
 * @date    January 2014
 * @brief   MIDI
 *
  ******************************************************************************/


/* Includes -------------------------------------------------------------------*/
#include "dma.h"
#include "utl.h"

static uint32_t uart_dma_tc_cnt;

static DMA_Stream_TypeDef *const 
 dma2_streams[] = {
    DMA2_Stream1,
    DMA2_Stream2,
    DMA2_Stream3,
    DMA2_Stream4,
    DMA2_Stream5,
    DMA2_Stream6,
    DMA2_Stream7,
};

extern void
dma_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
}

extern void
dma_init_dma2_chx(uint32_t str, DMA_Stream_TypeDef const *cfg)
{
    DMA_Stream_TypeDef *rstr;

    rstr = dma2_streams[str - 1];

    rstr->CR = 0;
    rstr->PAR = cfg->PAR;
    rstr->M0AR = cfg->M0AR;
    rstr->NDTR = cfg->NDTR;
    rstr->CR = cfg->CR;
}

extern void
uart_configure_dma2(uint8_t *src, uint16_t nbytes)
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
    utl_enable_irq(DMA2_Stream2_IRQn);
}

void DMA2_Stream2_IRQHandler(void) {
    uint32_t isr;

    isr = DMA2->LISR;

    // clear status reg 
    DMA2->LIFCR = isr;

    if (isr & DMA_LISR_TCIF2) {
        //callback(uart_data);

        uart_dma_tc_cnt++;    
    }
}