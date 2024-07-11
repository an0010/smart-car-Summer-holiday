#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "gd32f10x.h"
#include "systick.h"
#include "led_driver.h"
#include "key_driver.h"
#include "oled_driver.h"
#include "uart_driver.h"




uint8_t transmitter_buffer1[] = "HELLOWORLD";//���巢������
uint8_t receiver_buffer1[32];//�����������
#define ARRAYNUM1(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE1   (ARRAYNUM1(transmitter_buffer1) - 1)//�����С
uint8_t transfersize1 = TRANSMIT_SIZE1;

uint8_t receivesize1 = 10;    //����
__IO uint8_t  txcount1 = 0; 
__IO uint16_t rxcount1 = 0; 

//-------------------------------------------------------------------------------------

 uint8_t  USART_Rx_Buf[USART_RECV_BUF_SIZE];  // ���ڱ��� RX ���յ�������
 uint16_t USART_Rx_Len = 0;                   // ���ڱ��� RX ���յ������ݵĸ���
 uint16_t USART_Rx_Count = 0;                 // ���ڱ��� RX ��������ʱ�Ѿ����յ������ݸ���
 uint8_t  USART_Tx_Buf[USART_SEND_BUF_SIZE];  // ���ڱ��� Tx Ҫ���͵�����
 uint16_t USART_Tx_Len = 0;                   // ���ڱ��� Tx Ҫ���͵����ݵĸ���
 uint16_t USART_Tx_Count = 0;                 // ���ڱ��� Tx Ҫ���͵�����ʱ�Ѿ����͵����ݸ���


/**
  * @brief �ض���c�⺯��printf��USART1
  * @retval None
  */
int fputc(int ch,FILE *p)  
{
	usart_data_transmit(USART1,(unsigned char)ch);
	while(usart_flag_get(USART1, USART_FLAG_TC)==RESET);
	return ch;
}

/**
  * @brief �ض���c�⺯��getchar,scanf��USART2
  * @retval None
  */
int fgetc(FILE *f)
{
    uint8_t ch2 = 0;
    ch2 = usart_data_receive(USART2);
    return ch2;
}


/********************************************************************
* ��������void UART1_init(void)
* ��  ��: uint32_t Baud_rate  ������
* ��  �ã�USART1_Init(115200);
* USART1��ʼ����ʹ��PA2(TX),PA3(RX)�ţ�115200�����ʣ���У�飬8λ���ݣ�1λֹͣ
********************************************************************/
void USART1_Init(uint32_t Baud_rate)
{
    rcu_periph_clock_enable(RCU_GPIOA);//ʹ��GPIOAʱ��
    rcu_periph_clock_enable(RCU_USART1);//ʹ��USART1ʱ��

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_2);        //PA2  TX���
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_3);  //PA3  RX����
    /* USART configure */
    usart_deinit(USART1);
    usart_baudrate_set(USART1,Baud_rate);
	usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
	usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_enable(USART1);//ʹ��USART1	
    
    nvic_irq_enable(USART1_IRQn, 0, 1);             //ʹ��USART1�ж�
	usart_interrupt_enable(USART1, USART_INT_RBNE); //�����жϴ�
}
/********************************************************************
* ��������void UART2_init(void)
* ��  ��: uint32_t Baud_rate  ������
* ��  �ã�USART2_Init(115200);
* USART2��ʼ����ʹ��PB10(TX),PB11(RX)�ţ�115200�����ʣ���У�飬8λ���ݣ�1λֹͣ
********************************************************************/
void USART2_Init(uint32_t Baud_rate)
{
    rcu_periph_clock_enable(RCU_GPIOB);//ʹ��GPIOAʱ��
    rcu_periph_clock_enable(RCU_USART2);//ʹ��USART1ʱ��

    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);        //PB10  TX���
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_11);  //PB11  RX����
    /* USART configure */
    usart_deinit(USART2);
    usart_baudrate_set(USART2,Baud_rate);
		usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_NONE);
		usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_enable(USART2);//ʹ��USART2
    
    nvic_irq_enable(USART2_IRQn, 0, 0);             //ʹ��USART2�жϣ���ռ���ȼ��ʹ����ȼ���ԽСԽ��
		usart_interrupt_enable(USART2, USART_INT_RBNE); //�շ��жϴ�
}



void USART_Sent_Byte(uint32_t usart_periph,uint8_t byte)
{
    usart_data_transmit(USART2, byte);	    					//ͨ��USART2����
    while(RESET == usart_flag_get(USART2, USART_FLAG_TBE));		//�жϻ������Ƿ��Ѿ�����
}


//����Ϊͨ��
void USART_Sent_Data(uint32_t usart_periph ,uint8_t *data)
{
    uint32_t len;
    len = sizeof(*data) / sizeof(data[0]);
    while(data && len)
	{
		USART_Sent_Byte(usart_periph,*data);
		data++;		//ָ��������һλ
        len--;
	}
}

void USART_Sent_String(uint32_t usart_periph,char *str)
{
	while(str && (*str))
     {
		USART_Sent_Byte(usart_periph,(uint8_t)*str);
		str++;		// ָ������һ���ƶ�
     }
}



void Test_UASRT1(void)
{
	//USART�������
	USART1_Init(115200);                //��ʼ��USART 115200 ʹ�� �����������жϽ���
	USART2_Init(115200);                //��ʼ��USART 115200 ʹ�� �����������жϽ���

  OLED_Init();                         // OLED��ʼ��
  OLED_P6x8Str(10, 0,"Test USART1");  // �ַ���
	delay_1ms(50);
	printf(" USART1 Init ok \n");
	
	while(1)
	{
		if(rxcount1 >= receivesize1)//������10���ֽڣ���USART0_IRQHandler�����м���
		{
			rxcount1=0;
			txcount1=0;
			usart_interrupt_enable(USART1, USART_INT_TBE);//�����жϴ�
			while(txcount1 < transfersize1);//�ȴ�������ɣ���USART0_IRQHandler�����м���
			while (RESET == usart_flag_get(USART1, USART_FLAG_TC));//��������ж�
			usart_interrupt_enable(USART1, USART_INT_RBNE);//�����жϴ�
			printf("\nReceive1 OK \n");
		}
	}
}

int32_t Test_UASRT2(void)
{
   
	// printf("USART2 Init OK \n");
  	// USART_Sent_String(USART2,"USART2 Init OK\n");

  return 0;
	
}


void Data_parsing()   //???????
{
	//??????0xFA xx xx xx 0xAF
	
	;
}


