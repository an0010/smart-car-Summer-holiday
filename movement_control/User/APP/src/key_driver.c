#include "key_driver.h"
#include "led_driver.h"
#include "gd32f10x.h"

/*KEY��ʼ��*/
void key_init(void)
{
	/* ��KEY1��ʱ�� */
	// rcu_periph_clock_enable(KEY1_GPIO_PORT);
	rcu_periph_clock_enable(KEY1_GPIO_CLK);
	/* ����KEY1��GPIOģʽ */
	gpio_init(KEY1_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY1_PIN);

	/* ��KEY2��ʱ�� */
	// rcu_periph_clock_enable(KEY2_GPIO_PORT);
	rcu_periph_clock_enable(KEY2_GPIO_CLK);
	/* ����KEY2��GPIOģʽ */
	gpio_init(KEY2_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY2_PIN);

	/* ��KEY3��ʱ�� */
	// rcu_periph_clock_enable(KEY3_GPIO_PORT);
	rcu_periph_clock_enable(KEY3_GPIO_CLK);
	/* ����KEY1��GPIOģʽ */
	gpio_init(KEY3_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY3_PIN);

	/* ��KEY4��ʱ�� */
	// rcu_periph_clock_enable(KEY4_GPIO_PORT);
	rcu_periph_clock_enable(KEY1_GPIO_CLK);
	/* ����KEY1��GPIOģʽ */
	gpio_init(KEY4_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY4_PIN);

	/* ��KEY5��ʱ�� */
	// rcu_periph_clock_enable(KEY5_GPIO_PORT);
	rcu_periph_clock_enable(KEY5_GPIO_CLK);
	/* ����KEY1��GPIOģʽ */
	gpio_init(KEY5_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY5_PIN);
}

/*
@�������ƣ�Read_key
@����˵�����ж���ذ����Ƿ���
@����˵����num :�������
@�������أ�״̬ 1������ 0��û����

@���÷�����if(Read_key(KEY0)) KEY0�Ƿ���
@��    ע����
*/

uint8_t Read_key(key_num num)
{
	switch (num)
	{
	case 0:
		if (gpio_input_bit_get(KEY1_GPIO_PORT, KEY1_PIN) == RESET)
		{
			if (gpio_input_bit_get(KEY1_GPIO_PORT, KEY1_PIN) == RESET)
				return 1;
		}
		break;

	case 1:
		if (gpio_input_bit_get(KEY2_GPIO_PORT, KEY2_PIN) == RESET)
		{
			if (gpio_input_bit_get(KEY2_GPIO_PORT, KEY2_PIN) == RESET)
				return 1;
		}
		break;

	case 2:
		if (gpio_input_bit_get(KEY3_GPIO_PORT, KEY3_PIN) == RESET)
		{
			if (gpio_input_bit_get(KEY3_GPIO_PORT, KEY3_PIN) == RESET)
				return 1;
		}
		break;
	case 3:
		if (gpio_input_bit_get(KEY4_GPIO_PORT, KEY4_PIN) == RESET)
		{
			if (gpio_input_bit_get(KEY4_GPIO_PORT, KEY4_PIN) == RESET)
				return 1;
		}
		break;

	case 4:
		if (gpio_input_bit_get(KEY5_GPIO_PORT, KEY5_PIN) == RESET)
		{
			if (gpio_input_bit_get(KEY5_GPIO_PORT, KEY5_PIN) == RESET)
				return 1;
		}
		break;
	}
	return 0;
}
