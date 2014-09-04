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

/* Private typedefs -----------------------------------------------------------*/
typedef struct {
    uint8_t data[RX_BUFFER_SIZE];
    uint32_t tail;
    uint32_t head;
} uart_buf_t;

/* Private variables ----------------------------------------------------------*/
static uart_buf_t uart_buf;
static uint32_t uart_recv_cnt;
static uart_recv_callback_fn callback;

/* Function Declarations ------------------------------------------------------*/
extern void 
uart_init(uint32_t baudrate, uart_recv_callback_fn callback_fn)
{
    uint32_t i;
    USART_InitTypeDef usart;
    NVIC_InitTypeDef nvic;

    callback = callback_fn;

    uart_buf.tail = 0;
    uart_buf.head = 0;
    uart_recv_cnt = 0;

    for (i = 0; i < RX_BUFFER_SIZE; i++) {
        uart_buf.data[i] = 0;
    }

    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    /* Configure USART Tx/Rx as alternate function  */
    iox_configure_pin(iox_port_b, 6, iox_mode_af, iox_type_pp, 
                        iox_speed_fast, iox_pupd_up);
    iox_configure_pin(iox_port_b, 7, iox_mode_af, iox_type_pp,
                        iox_speed_fast, iox_pupd_up);

    /* USART configuration */
    usart.USART_BaudRate = baudrate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USART1 Interrupt */
    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_Init(USART1, &usart);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}

extern void 
uart_send_data(unsigned char *buf, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        USART1->DR = buf[i];
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}

extern bool 
uart_buffer_empty(void)
{
    return (uart_buf.head == uart_buf.tail);
}

extern uint8_t 
uart_read_byte(void)
{
    uint8_t data = 0;

    if (uart_buf.head != uart_buf.tail) {
        data = uart_buf.data[uart_buf.tail];
        uart_buf.tail = (uart_buf.tail + 1) % RX_BUFFER_SIZE;
    }
    return data;
}

void USART1_IRQHandler(void)
{
    uint8_t data;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        data = USART_ReceiveData(USART1);
        callback(data);

        USART1->SR &= ~USART_SR_RXNE;
        uart_recv_cnt++;
    }
}