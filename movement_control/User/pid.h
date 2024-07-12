#ifndef _PID_H
#define _PID_H
#include <stdint.h>

// void pid_straight(int E2,int E3,int PWM,int* SUM_pid_straight,int* PWM2,int* PWM3);
void pid_speed(int32_t E2, int32_t E3, int32_t ENC, int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3, int32_t* PWM2, int32_t *PWM3, int32_t *last_ENC__1_2, int32_t *last_ENC__1_3);
void pid_speed_1_motor(int32_t E, int32_t ENC, int32_t* PWM, int32_t* last_ENC);
void pid_speed_both_ENC(int32_t E2, int32_t E3, int32_t ENC_2,int32_t ENC_3 , int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3, int32_t* PWM2, int32_t *PWM3, int32_t *last_ENC__1_2, int32_t *last_ENC__1_3);
void pid_closing_ball(int32_t E2, int32_t E3, int32_t ENC, 
    int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3,
    int32_t* PWM2, int32_t *PWM3,
    int32_t* last_ENC__1_2, int32_t* last_ENC__1_3,
    int32_t ball_x);
void pid_shot(int32_t E2, int32_t E3, int32_t ENC, 
    int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3,
    int32_t* PWM2, int32_t *PWM3,
    int32_t* last_ENC__1_2, int32_t* last_ENC__1_3,
    int32_t bias);
void pid_fine_turn_ball(
    int32_t* PWM2, int32_t *PWM3,
    int32_t ball_x);
void queue_push(int32_t* queue, int32_t value, int32_t length);
int queue_sum(int32_t* queue, int32_t length);
#endif
