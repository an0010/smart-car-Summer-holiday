#include "led_driver.h"
#include "systick.h"
#include "gd32f10x.h"

/*��ʼ��LED��������PB15 ������� �ٶ�50MHZ*/
void led_init(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOB); // ʹ��GPIOBʱ��
    /* configure led GPIO port */
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15); // ��ʼ��LED��Ӧ��GPIO

    gpio_bit_set(GPIOB,GPIO_PIN_15);
}

/*LED��*/
void led_on(void)
{
		gpio_bit_reset(GPIOB,GPIO_PIN_15);
}
/*LED�ر�*/
void led_off(void)
{
		gpio_bit_set(GPIOB,GPIO_PIN_15);
}
/*LED״̬��ת*/
void led_toggle(void)
{
    gpio_bit_write(GPIOB, GPIO_PIN_15,
                   (bit_status)(1 - gpio_input_bit_get(GPIOB, GPIO_PIN_15)));
}
