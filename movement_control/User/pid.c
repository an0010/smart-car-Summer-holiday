#include "pid.h"

// void pid_straight(int E2,int E3,int PWM,int* SUM_pid_straight,int* PWM2,int* PWM3){
//     const float lambda = 1.0;
//     int ave = (E2+E3)/2;
//     (*PWM2) = PWM + lambda*(E2-ave);
//     (*PWM3) = PWM - lambda*(E3-ave);
//     (*PWM2) = (*PWM2)>2000?2000:(*PWM2);
//     (*PWM3) = (*PWM3)>2000?2000:(*PWM3);
//     (*PWM2) = (*PWM2)<-2000?-2000:(*PWM2);
//     (*PWM3) = (*PWM3)<-2000?-2000:(*PWM3);

// }

void pid_speed(int32_t E2, int32_t E3, int32_t ENC, int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3, int32_t* PWM2, int32_t *PWM3, int32_t last_ENC__1_2, int32_t last_ENC__1_3){

    const int register_num = 50;
    const float K = 10.0;
    const float lambda = 20 * K;
    const float lambda_p = 5 * K;
    const float lambda_i = 1 * K;
    const float lambda_d = 0.1 * K;

    const int MAX_PWM = 3000;

    int pwm2,pwm3;
    pwm2 = *PWM2;
    pwm3 = *PWM3;
    
    int loss2,loss3;
    E2 = -E2;
    loss2 = ENC - E2;
    loss3 = ENC - E3;

    int sum_loss2,sum_loss3;
    
    queue_push(SUM_pid_speed_2, loss2, register_num);
    queue_push(SUM_pid_speed_3, loss3, register_num);

    sum_loss2 = queue_sum(SUM_pid_speed_2, register_num);
    sum_loss3 = queue_sum(SUM_pid_speed_3, register_num);

    int delta_2; 
    delta_2 = E2 - last_ENC__1_2;
    int delta_3; 
    delta_3 = E3 - last_ENC__1_3;

    pwm2 = lambda*ENC + lambda_p*loss2 +lambda_i*sum_loss2+lambda_d*delta_2;
    pwm3 = lambda*ENC + lambda_p*loss3 +lambda_i*sum_loss3+lambda_d*delta_3;

    pwm2 = pwm2>MAX_PWM?MAX_PWM:pwm2;
    pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
    pwm2 = pwm2<-MAX_PWM?-MAX_PWM:pwm2;
    pwm3 = pwm3<-MAX_PWM?-MAX_PWM:pwm3;

    *PWM2 = -pwm2;
    *PWM3 = pwm3;
}

void pid_speed_both_ENC(int32_t E2, int32_t E3, int32_t ENC_2,int32_t ENC_3 , int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3, int32_t* PWM2, int32_t *PWM3, int32_t last_ENC__1_2, int32_t last_ENC__1_3){
    
        const int register_num = 50;
        const float K = 10.0;
        const float lambda = 20 * K;
        const float lambda_p = 5 * K;
        const float lambda_i = 1 * K;
        const float lambda_d = 0.1 * K;
    
        const int MAX_PWM = 3000;
    
        int pwm2,pwm3;
        pwm2 = *PWM2;
        pwm3 = *PWM3;
        
        int loss2,loss3;
        E2 = -E2;
        loss2 = ENC_2 - E2;
        loss3 = ENC_3 - E3;
    
        int sum_loss2,sum_loss3;
        
        queue_push(SUM_pid_speed_2, loss2, register_num);
        queue_push(SUM_pid_speed_3, loss3, register_num);
    
        sum_loss2 = queue_sum(SUM_pid_speed_2, register_num);
        sum_loss3 = queue_sum(SUM_pid_speed_3, register_num);
    
        int delta_2; 
        delta_2 = E2 - last_ENC__1_2;
        int delta_3; 
        delta_3 = E3 - last_ENC__1_3;
    
        pwm2 = lambda*ENC_2 + lambda_p*loss2 +lambda_i*sum_loss2+lambda_d*delta_2;
        pwm3 = lambda*ENC_3 + lambda_p*loss3 +lambda_i*sum_loss3+lambda_d*delta_3;
    
        pwm2 = pwm2>MAX_PWM?MAX_PWM:pwm2;
        pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
        pwm2 = pwm2<-MAX_PWM?-MAX_PWM:pwm2;
        pwm3 = pwm3<-MAX_PWM?-MAX_PWM:pwm3;
    
        *PWM2 = -pwm2;
        *PWM3 = pwm3;
}

void queue_push(int32_t* queue, int32_t value, int32_t length){
    int i = 0;
		for(i = length-1; i>0; i--){
        queue[i] = queue[i-1];
    }
    queue[0] = value;
}

int queue_sum(int32_t* queue, int32_t length){
    int sum = 0;
		int i = 0;
    for(i = 0; i<length; i++){
        sum += queue[i];
    }
    return sum;
}
