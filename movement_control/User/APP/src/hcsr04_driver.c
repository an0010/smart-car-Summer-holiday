#include "hcsr04_driver.h"
#include <stdio.h>
#include <stdint.h>
#include "gd32f10x.h"
#include "systick.h"
#include "led_driver.h"
#include "key_driver.h"
#include "oled_driver.h"
#include "uart_driver.h"

uint32_t TimeCounter = 0; // ��ʱ��������

/*
1.����:
	  ������ģ���뿪�����öŰ�������������PB9��Trig, PB8��Echo, GND��GND 5V��VCC��
2.ʵ��:
	  ���س���ȫ�����У���װOLED��Ļ����Ļ��ʾ����ֵ��˵��ʵ��ɹ�
*/
/*-----------------------------------------------------------
��ʱ����ʼ�� ��ʼ����ʱ��2 ��ʱʱ��Ϊ10us ��������������ʱ
ϵͳ��Ƶ72MHz
-----------------------------------------------------------*/
/**************************************************
 * ������: TIM2_Init(u16 arr,u16 psc)
 * ��  ��: ��ʱ��2��ʼ����������
 * ����ֵ: װ��ֵarr��Ԥ��Ƶϵ��ֵpsc
 ***************************************************/
void TIM2_Init(uint16_t arr, uint16_t psc)
{
	/* TIMER2 configuration */
	timer_parameter_struct Timer_initpara;
	rcu_periph_clock_enable(RCU_TIMER2); // ʹ��RCU_TIMER2ʱ��

	timer_deinit(TIMER2);

	/* initialize TIMER init parameter struct */
	timer_struct_para_init(&Timer_initpara);

	Timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	Timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	Timer_initpara.counterdirection = TIMER_COUNTER_UP;
	Timer_initpara.period = arr;	// 9;
	Timer_initpara.prescaler = psc; // 77;
	Timer_initpara.repetitioncounter = 0;
	timer_init(TIMER2, &Timer_initpara);

	// �ֶ����һ�¸����жϱ�־λ������ճ�ʼ����ɾͽ����ж�
	timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

	/* TIMER2 channel control update interrupt enable */
	timer_interrupt_enable(TIMER2, TIMER_INT_UP);

	/* enable and set key EXTI interrupt to the specified priority */
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(TIMER2_IRQn, 1U, 1U);

	timer_enable(TIMER2);
}

void Ultrasonic_Init()
{
	/*--------------------------------------------------------------------
	��ʼ�� ������ģ������
	--------------------------------------------------------------------*/
	rcu_periph_clock_enable(RCU_GPIOB);										// ʹ��GPIOBʱ��
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);		// Trig - PB11���
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8); // Echo - PB10����
	/*Ԥ������Trig����*/
	gpio_bit_write(GPIOB, GPIO_PIN_9, RESET);

	TIM2_Init(9, 77); // ���ڼ���
}

uint32_t HalTime1=0, HalTime2=0;
uint32_t Get_Distance(void) // ����ʱ�䵥λ10us
{
	uint32_t Distance = 0;
	/*��Trig��������ʮ��*/
	gpio_bit_write(GPIOB, GPIO_PIN_9, SET);
	delay_us(10);
	gpio_bit_write(GPIOB, GPIO_PIN_9, RESET);
	//���Echo�����ǵ͵�ƽ��һֱ�ȴ���ֱ��ΪEcho����Ϊ�ߵ�ƽ��Ϊ�ߵ�ƽ��˵�����ܵ��˷��ص��ź�
	while (gpio_input_bit_get(GPIOB, GPIO_PIN_8) == 0);
	//��¼�´�ʱ��ʱ��ֵ��10us��ʱ�����ۼӵı���������ʱ�䵥λΪ10us��
	HalTime1 = TimeCounter;
	//�ȴ��ߵ�ƽʱ�����
	while (gpio_input_bit_get(GPIOB, GPIO_PIN_8) == 1);

	//����ʱ�� �ж�ʱ������ǲ��Ǵ�ͷ��ʼ�ۼ��ˣ�����������Χʱ���0��ʼ�ۼӣ�
	if (TimeCounter > HalTime1)
	{
		//����ߵ�ƽ��ʱ�䳤��
		HalTime2 = TimeCounter - HalTime1;
		// ���뵥λcm,����340M/S��ʱ��*�ٶ�/2=����
		Distance = (uint32_t)(((float)HalTime2 * 17) / 100);
	}
	return Distance;
}

void Test_Ultrasonic()
{
	uint16_t Dis = 0.0;
	char txt[32];
	Ultrasonic_Init();
	OLED_Init();						 // OLED��ʼ��
	OLED_P6x8Str(10, 0, "Test HCSR04 "); // �ַ���
	delay_1ms(5);
	while (1)
	{
		Dis = Get_Distance();
		sprintf(txt, "Dis=%3d cm", Dis);
		OLED_P8x16Str(10, 2, txt); // ��ʾ�ַ���
		printf("Distance:%dcm\n", Dis);
		led_toggle();
		delay_1ms(100);
	}
}
