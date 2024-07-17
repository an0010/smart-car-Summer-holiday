#ifndef   __USART_DRIVER_H
#define   __USART_DRIVER_H

#include <stdint.h>


#define USART_RECV_BUF_SIZE 64// ���ڽ��ջ��泤��
#define USART_SEND_BUF_SIZE 64// ���ڷ��ͻ��泤��

void USART1_Init(uint32_t Baud_rate);
void USART2_Init(uint32_t Baud_rate);
void USART_Sent_Byte(uint32_t usart_periph,uint8_t byte);
void USART_Sent_Data(uint32_t usart_periph ,uint8_t *data);
void USART_Sent_String(uint32_t usart_periph,char *str);

void Test_UASRT1(void);
int32_t Test_UASRT2(void);
void Data_parsing(void);

void send_string(char *str);

#endif
