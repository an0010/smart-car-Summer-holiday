#include "tim_driver.h"
#include "gd32f10x.h"

/*-----------------------------------------------------------
��ʱ����ʼ�� ��ʼ����ʱ��2 ��ʱʱ��Ϊ1000000us=1000ms=1s 
ϵͳ��Ƶ72MHz
-----------------------------------------------------------*/
void Timer_Init(void)
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
    Timer_initpara.period = 7199;
    Timer_initpara.prescaler = 9999;
    Timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &Timer_initpara);

    // �ֶ��Ѹ����жϱ�־λ���һ�£�����ճ�ʼ����ɾͽ����ж�
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

    /* TIMER2 channel control update interrupt enable */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    /* enable and set key EXTI interrupt to the specified priority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(TIMER2_IRQn, 1U, 1U);

    timer_enable(TIMER2);
}
