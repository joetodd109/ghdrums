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