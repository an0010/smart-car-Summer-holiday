#include "Photoelectric.h"
#include "gd32f10x.h"

void sensor_init(void)
{
    /* ��sensor1��ʱ�� */
    // rcu_periph_clock_enable(sensor1_GPIO_PORT);
    rcu_periph_clock_enable(sensor1_GPIO_CLK);
    /* ����sensor1��GPIOģʽ */
    gpio_init(sensor1_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, sensor1_PIN);

    /* ��sensor2��ʱ�� */
    // rcu_periph_clock_enable(sensor2_GPIO_PORT);
    rcu_periph_clock_enable(sensor2_GPIO_CLK);
    /* ����sensor2��GPIOģʽ */
    gpio_init(sensor2_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, sensor2_PIN);

    /* ��sensor3��ʱ�� */
    // rcu_periph_clock_enable(sensor3_GPIO_PORT);
    rcu_periph_clock_enable(sensor3_GPIO_CLK);
    /* ����sensor1��GPIOģʽ */
    gpio_init(sensor3_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, sensor3_PIN);

    /* ��sensor4��ʱ�� */
    // rcu_periph_clock_enable(sensor4_GPIO_PORT);
    rcu_periph_clock_enable(sensor1_GPIO_CLK);
    /* ����sensor1��GPIOģʽ */
    gpio_init(sensor4_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, sensor4_PIN);
}

uint8_t Read_sensor(sensor_num num)
{
    switch (num)
    {
    case 0:
        if (gpio_input_bit_get(sensor1_GPIO_PORT, sensor1_PIN) == RESET)
            return 1;
        break;
    case 1:
        if (gpio_input_bit_get(sensor2_GPIO_PORT, sensor2_PIN) == RESET)
            return 1;
        break;
    case 2:
        if (gpio_input_bit_get(sensor3_GPIO_PORT, sensor3_PIN) == RESET)
            return 1;
        break;
    case 3:
        if (gpio_input_bit_get(sensor4_GPIO_PORT, sensor4_PIN) == RESET)
            return 1;
        break;
        }
    return 0;
}
