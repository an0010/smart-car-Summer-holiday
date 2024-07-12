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

int32_t receive_ball_cx;
int32_t receive_gate_cx;
int32_t receive_ball_dis_flag;

void clear_array(int32_t* array, int32_t length) {
	int i = 0;
	for (i = 0; i < length; i++) {
		array[i] = 0;
	}
}

//return [0:center; 1:slightly right; 2:far right; -1:slightly left; -2:far left]
int Judgeballposi(int ball) {
	int centerposi = 82;
	if (ball >= 0.65 * centerposi && ball <= 1.35 * centerposi) {
		return 0;
	} else if (ball < 0.65 * centerposi && ball >= 0.45 * centerposi) {
		return -1;
	} else if (ball > 1.35 * centerposi && ball <= 1.55 * centerposi) {
		return 1;
	} else if (ball < 0.45 * centerposi) {
		return -2;
	} else {
		return 2;
	}
}

int gate_center(int gate) {
	int centerposi = 82;
	if (gate >= 0.6 * centerposi && gate <= 1.4 * centerposi) {
		return 1;
	} else {
		return 0;
	}
}

void UpdateState(int ball, int gate, int ball_status, int* pstate) {
    int currentstate = *pstate;
    if (currentstate == 0) {
		if (ball_status == 1) {
			*pstate = 1;
		} else if (ball_status == 2) {
			*pstate = 3;
		}
	} else if (currentstate == 1) {
		if (ball_status == 2) {
			if (Judgeballposi(ball) == 0) {
				*pstate = 4;
			} else {
				*pstate = 3;
			}
		} else if (ball_status == 0) {
			*pstate = 0;
		}		
	} else if (currentstate == 3) {
		if (ball_status == 0 || ball_status == 1) {
			*pstate = 0;
		} else if (Judgeballposi(ball) == 0) {
			*pstate = 4;
		}
	} else if (currentstate == 4) {
		if (Judgeballposi(ball) != 0) {
			*pstate = 3;
		} else if (gate_center(gate) == 1) {
			*pstate = 2;
		}
	} else if (currentstate == 2) {
		if (ball_status == 0) {
			*pstate = 0;
		} else {
			if (gate_center(gate) == 0) {
				*pstate = 4;
			}
		}
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
	// uint16_t Dis = 0.0;

	// init the system clock
	systick_config();
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
	systick_config();						
	rcu_periph_clock_enable(RCU_AF);	

	// init the encoder
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
	OLED_P6x8Str(10, 0, "---KILLER ROBOT---");
	delay_1ms(50);
	USART2_Init(115200);   

	int32_t ball = 0;        // Ball position
	int32_t gate = 0;        // Gate position
	int32_t ball_status = 0; // 0: not found; 1: found, not reached; 2: reached
	int32_t lastball = 0;
	int32_t lastgate = 0;

	int32_t Enc1 = 0; //front wheel
	int32_t Enc2 = 0; //behind-right wheel
	int32_t Enc3 = 0; //behind-left wheel
	int32_t ENC = 0;  //goal speed

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
	int state = 0;   // state 0: finding ball; state 1: ball found; state 3/4: ball found, finding gate; state 2: going towards gate

	while (1) {

		if (Read_key(KEY1) == 1) {ENC += 1;}
		if (Read_key(KEY2) == 1) {ENC = 0;}
		if (Read_key(KEY3) == 1) {ENC -= 1;}
		led_toggle();

		// sensor_Value[0] = Read_sensor(sensor1);
		// sensor_Value[1] = Read_sensor(sensor2);
		// sensor_Value[2] = Read_sensor(sensor3);
		// sensor_Value[3] = Read_sensor(sensor4);

		Enc1 = ENC_Read(ENCODER1_TIMER);
		Enc2 = ENC_Read(ENCODER2_TIMER);
		Enc3 = ENC_Read(ENCODER3_TIMER);

		ball = receive_ball_cx;
		gate = receive_gate_cx;
		ball_status = receive_ball_dis_flag;

		UpdateState(ball, gate, ball_status, &state);

		//从左到右 1 0 2 3
		//flag为1：向右转

		if (state == 1) {
			// if (Judgeballposi(ball) == 0){
			// 	// Enc3 = -Enc1;
			// 	pid_speed(Enc1, Enc3, ENC, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// 	PWM2 = 0;
			// 	clear_array(SUM_pid_speed_turn_1, 50);
			// 	clear_array(SUM_pid_speed_turn_3, 50);
			// } else if (Judgeballposi(ball) == 1) {
			// 	// Enc3 = -Enc1;
			// 	pid_speed_both_ENC(Enc1, Enc3, ENC, ENC + 3, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// 	PWM2 = 0;
			// 	clear_array(SUM_pid_speed_turn_1, 50);
			// 	clear_array(SUM_pid_speed_turn_3, 50);
			// } else if (Judgeballposi(ball) == 2) {
			// 	// Enc3 = Enc1;
			// 	pid_speed(Enc1, Enc3,ENC * 0.5, SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// 	PWM1 = PWM3;
			// 	PWM2 = PWM3;
			// 	clear_array(SUM_pid_speed_1, 50);
			// 	clear_array(SUM_pid_speed_3, 50);
			// } else if (Judgeballposi(ball) == -1) {
			// 	// Enc3 = -Enc1;
			// 	pid_speed_both_ENC(Enc1, Enc3, ENC + 3, ENC, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// 	PWM2 = 0;
			// 	clear_array(SUM_pid_speed_turn_1, 50);
			// 	clear_array(SUM_pid_speed_turn_3, 50);
			// } else if (Judgeballposi(ball) == -2){
			// 	// Enc3 = Enc1;
			// 	pid_speed(Enc1, Enc3, ENC * (-0.5), SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// 	PWM1 = PWM3;
			// 	PWM2 = PWM3;
			// 	clear_array(SUM_pid_speed_1, 50);
			// 	clear_array(SUM_pid_speed_3, 50);
			// }
			if (Judgeballposi(lastball) < 0) {
				turn_flag = -1;
			} else {
				turn_flag = 1;
			}
			pid_closing_ball(Enc1, Enc3, ENC * 1.5, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3, ball);
			PWM2 = 0;
			clear_array(SUM_pid_speed_turn_1, 50);
			clear_array(SUM_pid_speed_turn_3, 50);
		} else if (state == 0) {
			// Enc3 = Enc1;
			// if (Judgeballposi(lastball) < 0) {
			// 	turn_flag = -1;
			// } else {
			// 	turn_flag = 1;
			// }
			pid_speed(Enc1, Enc3, ENC * turn_flag * 0.4, SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			PWM1 = PWM3;
			PWM2 = PWM3;
			clear_array(SUM_pid_speed_1, 50);
			clear_array(SUM_pid_speed_3, 50); 
		} else if (state == 3) {
			// if (Judgeballposi(ball) >= 1) {
			// 	// Enc3 = Enc1;
			// 	pid_speed(Enc1, Enc3, 0.5 * ENC, SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// 	PWM1 = PWM3;
			// 	PWM2 = PWM3;
			// 	clear_array(SUM_pid_speed_1, 50);
			// 	clear_array(SUM_pid_speed_3, 50);
			// } else if (Judgeballposi(ball) <= -1) {
			// 	// Enc3 = Enc1;
			// 	pid_speed(Enc1, Enc3,ENC*(-0.5), SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// 	PWM1 = PWM3;
			// 	PWM2 = PWM3;
			// 	clear_array(SUM_pid_speed_1, 50);
			// 	clear_array(SUM_pid_speed_3, 50);
			// }
			if (Judgeballposi(lastball) < 0) {
				turn_flag = -1;
			} else if(Judgeballposi(lastball) > 0){
				turn_flag = 1;
			}
			pid_fine_turn_ball(Enc1, Enc3, SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3, ball);
			PWM2 = 0;
			clear_array(SUM_pid_speed_1, 50);
			clear_array(SUM_pid_speed_3, 50);
		} else if (state == 4) {
				// Enc3 = Enc1;
				// pid_speed(Enc1, Enc3,ENC*(-1), SUM_pid_speed_turn_1, SUM_pid_speed_turn_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
				if (Judgeballposi(lastball) < 0) {
				turn_flag = -1;
				} else if(Judgeballposi(lastball) > 0){
				turn_flag = 1;
				}
				PWM1 = 0;
				PWM3 = 0;
				PWM2 = 250 * ENC;
				clear_array(SUM_pid_speed_1, 50);
				clear_array(SUM_pid_speed_3, 50);
		} else if (state == 2) {
			// Enc3 = -Enc1;
			// pid_speed(Enc1, Enc3, ENC, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3);
			// PWM2 = 0;
			// clear_array(SUM_pid_speed_turn_1, 50);
			// clear_array(SUM_pid_speed_turn_3, 50);
			if (Judgeballposi(lastball) < 0) {
				turn_flag = -1;
			} else if(Judgeballposi(lastball) > 0){
				turn_flag = 1;
			}
			pid_shot(Enc1, Enc3, ENC*1.5, SUM_pid_speed_1, SUM_pid_speed_3, &PWM1, &PWM3, last_ENC__1_2, last_ENC__1_3, 0);
			PWM2 = 0;
			clear_array(SUM_pid_speed_turn_1, 50);
			clear_array(SUM_pid_speed_turn_3, 50);
		}


		MotorCtrl3W(PWM1, PWM2, PWM3);

		lastball = ball;
		lastgate = gate;

		sprintf(txt, "ball: %d", ball);
		OLED_P6x8Str(0, 2, txt); // �ַ���
		sprintf(txt, "turn_flag: %d", turn_flag);
		OLED_P6x8Str(0, 4, txt); // �ַ���
		sprintf(txt, "status: %d, %d", ball_status, state);
		OLED_P6x8Str(0, 6, txt); // �ַ���

		led_toggle();
		delay_1ms(20);
	}
}