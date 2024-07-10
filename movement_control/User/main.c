#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "led_driver.h"
#include "key_driver.h"
#include "oled_driver.h"
#include "tim_driver.h"
#include "enc_driver.h"
#include "uart_driver.h"
#include "pwm_driver.h"
#include "Photoelectric.h"
#include "hcsr04_driver.h"
#include "pid.h"

void clear_array(int32_t* array, int32_t length);

int ball_center(int ball) {
	if (ball >= 30 && ball <= 50) {
		return 1;
	} else {
		return 0;
	}
}

int gate_center(int gate) {
	if (gate >= 25 && gate <= 50) {
		return 1;
	} else {
		return 0;
	}
}

/*!
	\brief      main function
	\param[in]  none
	\param[out] none
	\retval     none
*/
int main(void) {
	char txt[32];
	uint16_t Dis = 0.0;

	systick_config();
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
	systick_config();						
	rcu_periph_clock_enable(RCU_AF);	

	encoder1_config();
	encoder2_config();
	encoder3_config();

	led_init(); 
	key_init();	
	OLED_Init(); 
	delay_1ms(100);
	OLED_CLS();
	Motor_Init();
	OLED_Init();   
	OLED_P6x8Str(10, 0, "Test HCSR04 ");
	delay_1ms(50);
	
	USART2_Init(115200);   
	int32_t receive_num = 0;
	int32_t ball = 0;
	int32_t gate = 0;
	int32_t ball_status = 0; // 0: not found; 1: found, not reached; 2: reached

	int32_t Enc1 = 0;
	int32_t Enc2 = 0;
	int32_t Enc3 = 0;
	int32_t ENC = 0;	

	int PWM1 = 0;
	int PWM2 = 0;
	int PWM3 = 0;

	int32_t SUM_pid_speed_1[50] = {0};
	int32_t SUM_pid_speed_3[50] = {0};

	int32_t SUM_pid_speed_turn_1[50] = {0};
	int32_t SUM_pid_speed_turn_3[50] = {0};

	uint8_t sensor_Value[4];

	int32_t last_ENC__1_2 = 0;
	int32_t last_ENC__1_3 = 0;

	int32_t turn_flag = 0;

	int32_t search_time = 0;
	int counter = 0;
	int status = 0;  // status 0: avoiding obstacle; status 1: tracking
	int state = 0;   // state 0: finding ball; state 10/11: ball found, finding gate; state 2: going towards gate

	while (1) {

		if (Read_key(KEY1) == 1) {ENC += 1;}
		if (Read_key(KEY2) == 1) {ENC = 0;}
		if (Read_key(KEY3) == 1) {ENC -= 1;}
		led_toggle();

		// sensor_Value[0] = Read_sensor(sensor1);
		// sensor_Value[1] = Read_sensor(sensor2);
		// sensor_Value[2] = Read_sensor(sensor3);
		// sensor_Value[3] = Read_sensor(sensor4);

		Enc1 = ENC_Read(ENCODER3_TIMER);
		Enc2 = ENC_Read(ENCODER1_TIMER);
		Enc3 = ENC_Read(ENCODER2_TIMER);

        receive_num = Test_UASRT2();
		if (receive_num >= 0 && receive_num <= 100) {
			gate = receive_num;
		} else { 
			if (receive_num > 100 && receive_num <= 150) {
				ball = (receive_num - 100) * 2;
				ball_status = 1;
			} else if (receive_num > 150 && receive_num < 200) {
				ball = (receive_num - 150) * 2;
				ball_status = 2;
			} else {
                ball = -1;
				ball_status = 0;
			}
		}

        // judge state (whether to change state)
		if (state == 0) {
			if (ball_status == 2) {
				if (ball_center(ball) == 1) {
					state = 11;
				} else {
					state = 10;
				}
			}
		} else if (state == 10) {
			if (ball_status == 0 || ball_status == 1) {
				state = 0;
			} else if (ball_center(ball) == 1) {
				state = 11;
			} 
		} else if (state == 11) {
			// if (ball_status == 0) {
			// 	state = 0;
			// } else 
			if (ball_center(ball) == 0) {
				state = 10;
			} 
			else if (gate_center(gate) == 1) {
				state = 2;
			}
		} else if (state == 2) {
			if (ball_status == 0 /*|| ball_status == 1*/) {
				state = 0;
			} else  {
				if (gate_center(gate) == 0) {
					state = 11;
				}
				// } else {
				// 	state = 10;
				// }
			// }
		}
		}

		//从左到右 1 0 2 3
		//flag为1：向右转

		if (state == 0) {
            if (ball_status == 1) {
				if (ball >= 35 && ball <= 45) {
					// Enc3 = -Enc1;
					pid_speed(Enc1, Enc3, ENC, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
					PWM2 = 0;
					turn_flag = 0;
					search_time = 0;
					clear_array(SUM_pid_speed_turn_1, 50);
					clear_array(SUM_pid_speed_turn_3, 50);
				} else if (ball > 45 && ball <= 50){
					// Enc3 = -Enc1;
					pid_speed_both_ENC(Enc1, Enc3,ENC, ENC+2, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
					PWM2 = 0;
					turn_flag = 1;
					search_time = 0;
					clear_array(SUM_pid_speed_turn_1, 50);
					clear_array(SUM_pid_speed_turn_3, 50);
				} else if (ball > 50 && ball < 60){
					// Enc3 = -Enc1;
					pid_speed_both_ENC(Enc1, Enc3,ENC, ENC+4, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
					PWM2 = 0;
					turn_flag = 1;
					search_time = 0;
					clear_array(SUM_pid_speed_turn_1, 50);
					clear_array(SUM_pid_speed_turn_3, 50);
				} else if (ball > 60){
					// Enc3 = Enc1;
					pid_speed(Enc1, Enc3,ENC * 0.5, SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
					PWM1 = PWM3;
					PWM2 = PWM3;
					turn_flag = 1;
					search_time = 0;
					clear_array(SUM_pid_speed_1, 50);
					clear_array(SUM_pid_speed_3, 50);
				} else if (ball >= 30 && ball < 35){
					// Enc3 = -Enc1;
					pid_speed_both_ENC(Enc1, Enc3,ENC+2, ENC, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
					PWM2 = 0;
					turn_flag = -1;
					search_time = 0;
					clear_array(SUM_pid_speed_turn_1, 50);
					clear_array(SUM_pid_speed_turn_3, 50);
				} else if (ball >= 20 && ball < 30){
					// Enc3 = -Enc1;
					pid_speed_both_ENC(Enc1, Enc3,ENC+4, ENC, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
					PWM2 = 0;
					turn_flag = -1;
					search_time = 0;
					clear_array(SUM_pid_speed_turn_1, 50);
					clear_array(SUM_pid_speed_turn_3, 50);
				} else if (ball < 20 && ball >= 0){
					// Enc3 = Enc1;
					pid_speed(Enc1, Enc3,ENC*(-0.5), SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
					PWM1 = PWM3;
					PWM2 = PWM3;
					turn_flag = -1;
					search_time = 0;
					clear_array(SUM_pid_speed_1, 50);
					clear_array(SUM_pid_speed_3, 50);
				}
			} else if (ball_status == 0) {
				// Enc3 = Enc1;
				pid_speed(Enc1, Enc3,ENC*(-0.5), SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
				PWM1 = PWM3;
				PWM2 = PWM3;
				turn_flag = -1;
				search_time = 0;
				clear_array(SUM_pid_speed_1, 50);
				clear_array(SUM_pid_speed_3, 50);
			} else {
				PWM1 = 0;
				PWM2 = 0;
				PWM3 = 0;
			}
		} else if (state == 10) {
			if (ball > 45) {
				// Enc3 = Enc1;
				pid_speed(Enc1, Enc3,0.5 * ENC, SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
				PWM1 = PWM3;
				PWM2 = PWM3;
				turn_flag = -1;
				search_time = 0;
				clear_array(SUM_pid_speed_1, 50);
				clear_array(SUM_pid_speed_3, 50);
			} else if ( ball < 35 && ball >= 0) {
				// Enc3 = Enc1;
				pid_speed(Enc1, Enc3,ENC*(-0.5), SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
				PWM1 = PWM3;
				PWM2 = PWM3;
				turn_flag = -1;
				search_time = 0;
				clear_array(SUM_pid_speed_1, 50);
				clear_array(SUM_pid_speed_3, 50);
			}
		} else if (state == 11) {
				// Enc3 = Enc1;
				// pid_speed(Enc1, Enc3,ENC*(-1), SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
				PWM1 = 0;
				PWM3 = 0;
				PWM2 = 1500;
				turn_flag = -1;
				search_time = 0;
				clear_array(SUM_pid_speed_1, 50);
				clear_array(SUM_pid_speed_3, 50);
		} else if (state == 2) {
			Enc3 = -Enc1;
			pid_speed(Enc1, Enc3, ENC, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			PWM2 = 0;
			turn_flag = 0;
			search_time = 0;
			clear_array(SUM_pid_speed_turn_1, 50);
			clear_array(SUM_pid_speed_turn_3, 50);
		}


		MotorCtrl3W(PWM2, PWM3, PWM1);

		sprintf(txt, "ball: %d", ball);
		OLED_P6x8Str(0, 2, txt); // �ַ���
		sprintf(txt, "gate: %d", gate);
		OLED_P6x8Str(0, 4, txt); // �ַ���
		sprintf(txt, "status: %d, %d", ball_status, state);
		OLED_P6x8Str(0, 6, txt); // �ַ���

		led_toggle();
		delay_1ms(50);
	}
}

void clear_array(int32_t* array, int32_t length) {
	int i = 0;
	for (i = 0; i < length; i++) {
		array[i] = 0;
	}
}