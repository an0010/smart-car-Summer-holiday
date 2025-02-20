#include "pid.h"

void pid_speed(int32_t E2, int32_t E3, int32_t ENC, int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3, int32_t* PWM2, int32_t *PWM3, int32_t *last_ENC__1_2, int32_t *last_ENC__1_3){

    const int register_num = 50;
    const float K = 10.0;
    const float lambda = 20 * K;
    const float lambda_p = 5 * K;
    const float lambda_i = 0 * K;
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
    delta_2 = E2 - *last_ENC__1_2;
    int delta_3; 
    delta_3 = E3 - *last_ENC__1_3;

    pwm2 = lambda*ENC + lambda_p*loss2 +lambda_i*sum_loss2+lambda_d*delta_2;
    pwm3 = lambda*ENC + lambda_p*loss3 +lambda_i*sum_loss3+lambda_d*delta_3;

    pwm2 = pwm2>MAX_PWM?MAX_PWM:pwm2;
    pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
    pwm2 = pwm2<-MAX_PWM?-MAX_PWM:pwm2;
    pwm3 = pwm3<-MAX_PWM?-MAX_PWM:pwm3;

    *PWM2 = -pwm2;
    *PWM3 = pwm3;

    *last_ENC__1_2 = E2;
    *last_ENC__1_3 = E3;
}

void pid_speed_1_motor(int32_t E, int32_t ENC, int32_t* PWM, int32_t* last_ENC){

    static int32_t SUM_pid_speed[50] = {0};

    const int register_num = 50;
    const float K = 10.0;
    const float lambda = 5 * K;
    const float lambda_p = 3 * K;
    const float lambda_i = 0 * K;
    const float lambda_d = 0.0 * K;

    const int MAX_PWM = 3000;

    int pwm;
    pwm = *PWM;
    
    int loss;
    loss = ENC - E;

    int sum_loss;
    
    queue_push(SUM_pid_speed, loss, register_num);

    sum_loss = queue_sum(SUM_pid_speed, register_num);

    int delta; 
    delta = E - *last_ENC;

    pwm = lambda*ENC + lambda_p*loss +lambda_i*sum_loss+lambda_d*delta;

    pwm = pwm>MAX_PWM?MAX_PWM:pwm;
    pwm = pwm<-MAX_PWM?-MAX_PWM:pwm;

    *PWM = pwm;
    *last_ENC = E;
}

void pid_speed_3_motor(int32_t E1, int32_t E2, int32_t E3,
                      int32_t ENC, int32_t* PWM1, int32_t* PWM2, int32_t* PWM3){

    static int32_t SUM_pid_speed_1[50] = {0};
    static int32_t SUM_pid_speed_2[50] = {0};
    static int32_t SUM_pid_speed_3[50] = {0};

    static int32_t last_ENC_1 = 0;
    static int32_t last_ENC_2 = 0;
    static int32_t last_ENC_3 = 0;

    const int register_num = 50;
    const float K = 10.0;
    const float lambda = 5 * K;
    const float lambda_p = 3 * K;
    const float lambda_i = 0 * K;
    const float lambda_d = 0.0 * K;

    const int MAX_PWM = 3000;

    int pwm1, pwm2, pwm3;
    pwm1 = *PWM1;
    pwm2 = *PWM2;
    pwm3 = *PWM3;
    
    int loss1, loss2, loss3;
    loss1 = ENC - E1;
    loss2 = ENC - E2;
    loss3 = ENC - E3;

    int sum_loss1, sum_loss2, sum_loss3;
    
    queue_push(SUM_pid_speed_1, loss1, register_num);
    queue_push(SUM_pid_speed_2, loss2, register_num);
    queue_push(SUM_pid_speed_3, loss3, register_num);

    sum_loss1 = queue_sum(SUM_pid_speed_1, register_num);
    sum_loss2 = queue_sum(SUM_pid_speed_2, register_num);
    sum_loss3 = queue_sum(SUM_pid_speed_3, register_num);

    int delta1, delta2, delta3; 
    delta1 = E1 - last_ENC_1;
    delta2 = E2 - last_ENC_2;
    delta3 = E3 - last_ENC_3;

    pwm1 = lambda*ENC + lambda_p*loss1 +lambda_i*sum_loss1+lambda_d*delta1;
    pwm2 = lambda*ENC + lambda_p*loss2 +lambda_i*sum_loss2+lambda_d*delta2;
    pwm3 = lambda*ENC + lambda_p*loss3 +lambda_i*sum_loss3+lambda_d*delta3;

    pwm1 = pwm1>MAX_PWM?MAX_PWM:pwm1;
    pwm2 = pwm2>MAX_PWM?MAX_PWM:pwm2;
    pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
    pwm1 = pwm1<-MAX_PWM?-MAX_PWM:pwm1;
    pwm2 = pwm2<-MAX_PWM?-MAX_PWM:pwm2;
    pwm3 = pwm3<-MAX_PWM?-MAX_PWM:pwm3;

    *PWM1 = pwm1;
    *PWM2 = pwm2;
    *PWM3 = pwm3;
    last_ENC_1 = E1;
    last_ENC_2 = E2;
    last_ENC_3 = E3;
}

void pid_speed_both_ENC(int32_t E2, int32_t E3, int32_t ENC_2,int32_t ENC_3 , int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3, int32_t* PWM2, int32_t *PWM3, int32_t *last_ENC__1_2, int32_t *last_ENC__1_3){
    
        const int register_num = 50;
        const float K = 10.0;
        const float lambda = 20 * K;
        const float lambda_p = 5 * K;
        const float lambda_i = 0 * K;
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
        delta_2 = E2 - *last_ENC__1_2;
        int delta_3; 
        delta_3 = E3 - *last_ENC__1_3;
    
        pwm2 = lambda*ENC_2 + lambda_p*loss2 +lambda_i*sum_loss2+lambda_d*delta_2;
        pwm3 = lambda*ENC_3 + lambda_p*loss3 +lambda_i*sum_loss3+lambda_d*delta_3;
    
        pwm2 = pwm2>MAX_PWM?MAX_PWM:pwm2;
        pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
        pwm2 = pwm2<-MAX_PWM?-MAX_PWM:pwm2;
        pwm3 = pwm3<-MAX_PWM?-MAX_PWM:pwm3;
    
        *PWM2 = -pwm2;
        *PWM3 = pwm3;

        *last_ENC__1_2 = E2;
        *last_ENC__1_3 = E3;

}

void pid_closing_ball(int32_t E2, int32_t E3, int32_t ENC, 
    int32_t* SUM_pid_speed_2,int32_t* SUM_pid_speed_3,
    int32_t* PWM2, int32_t *PWM3,
    int32_t *last_ENC__1_2, int32_t *last_ENC__1_3,
    int32_t ball_x){
    
        if(ball_x == 200){
            ball_x = 80;
        }


        const int register_num = 50;
        const float K = 7.0;
        const float lambda = 7 * K;
        const float lambda_p = 1 * K;
        const float lambda_i = 0 * K;
        const float lambda_d = 0.0 * K;
        const float lambda_ball = 4;
    
        const int MAX_PWM = 5500;
    
        int pwm2,pwm3;
        pwm2 = *PWM2;
        pwm3 = *PWM3;
        
        int loss2,loss3;
        int E2_goal,E3_goal;
        E2 = -E2;
        E2_goal = ENC - (ball_x-80)*lambda_ball;
        E3_goal = ENC + (ball_x-80)*lambda_ball;
        loss2 = E2_goal - E2;
        loss3 = E3_goal - E3;
    
        int sum_loss2,sum_loss3;
        
        queue_push(SUM_pid_speed_2, loss2, register_num);
        queue_push(SUM_pid_speed_3, loss3, register_num);
    
        sum_loss2 = queue_sum(SUM_pid_speed_2, register_num);
        sum_loss3 = queue_sum(SUM_pid_speed_3, register_num);
    
        int delta_2; 
        delta_2 = E2 - *last_ENC__1_2;
        int delta_3; 
        delta_3 = E3 - *last_ENC__1_3;
    
        pwm2 = lambda*ENC + lambda_p*loss2 +lambda_i*sum_loss2+lambda_d*delta_2;
        pwm3 = lambda*ENC + lambda_p*loss3 +lambda_i*sum_loss3+lambda_d*delta_3;
    
        pwm2 = pwm2>MAX_PWM?MAX_PWM:pwm2;
        pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
        pwm2 = pwm2<0?0:pwm2;
        pwm3 = pwm3<0?0:pwm3;
    
        *PWM2 = -pwm2;
        *PWM3 = pwm3;

        *last_ENC__1_2 = E2;
        *last_ENC__1_3 = E3;
}

void pid_closing_ball_near(int32_t E2, int32_t E3, int32_t ENC, 
    int32_t* PWM2, int32_t *PWM3,
    int32_t ball_x){
    
        if(ball_x == 200){
            ball_x = 80;
        }

        static int32_t SUM_pid_2[50] = {0};
        static int32_t SUM_pid_3[50] = {0};

        static int32_t last_ENC_2 = 0;
        static int32_t last_ENC_3 = 0;

        const int register_num = 50;
        const float K = 7.0;
        const float lambda = 5 * K;
        const float lambda_p = 5 * K;
        const float lambda_i = 0 * K;
        const float lambda_d = 0.0 * K;
        const float lambda_ball = 0.7;
    
        const int MAX_PWM = 2000;
    
        int pwm2,pwm3;
        pwm2 = *PWM2;
        pwm3 = *PWM3;
        
        int loss2,loss3;
        int E2_goal,E3_goal;
        E2 = -E2;
        E2_goal = ENC - (ball_x-80)*lambda_ball;
        E3_goal = ENC + (ball_x-80)*lambda_ball;
        loss2 = E2_goal - E2;
        loss3 = E3_goal - E3;
    
        int sum_loss2,sum_loss3;
        
        queue_push(SUM_pid_2, loss2, register_num);
        queue_push(SUM_pid_3, loss3, register_num);
    
        sum_loss2 = queue_sum(SUM_pid_2, register_num);
        sum_loss3 = queue_sum(SUM_pid_3, register_num);
    
        int delta_2; 
        delta_2 = E2 - last_ENC_2;
        int delta_3; 
        delta_3 = E3 - last_ENC_3;
    
        pwm2 = lambda*ENC + lambda_p*loss2 +lambda_i*sum_loss2+lambda_d*delta_2;
        pwm3 = lambda*ENC + lambda_p*loss3 +lambda_i*sum_loss3+lambda_d*delta_3;
    
        pwm2 = pwm2>MAX_PWM?MAX_PWM:pwm2;
        pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
        pwm2 = pwm2<-MAX_PWM?-MAX_PWM:pwm2;
        pwm3 = pwm3<-MAX_PWM?-MAX_PWM:pwm3;
    
        *PWM2 = -pwm2;
        *PWM3 = pwm3;

        last_ENC_2 = E2;
        last_ENC_3 = E3;
}

void pid_shot(int32_t E2, int32_t E3, int32_t ENC, 
    int32_t* PWM2, int32_t *PWM3,
    int32_t bias){
    
        const int register_num = 50;
        const float K = 10.0;
        const float lambda = 20 * K;
        const float lambda_p = 5 * K;
        const float lambda_i = 0 * K;
        const float lambda_d = 0.0 * K;
        const float lambda_bias = 0.4;
    
        const int MAX_PWM = 5500;

        static int32_t SUM_pid_speed_2[50] = {0};
        static int32_t SUM_pid_speed_3[50] = {0};
        static int32_t last_ENC__1_2 = 0;
        static int32_t last_ENC__1_3 = 0;
    
        int pwm2,pwm3;
        pwm2 = *PWM2;
        pwm3 = *PWM3;
        
        int loss2,loss3;
        int E2_goal,E3_goal;
        E2 = -E2;
        E2_goal = ENC + bias*lambda_bias;
        E3_goal = ENC - bias*lambda_bias;
        loss2 = E2_goal - E2;
        loss3 = E3_goal - E3;
    
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

        last_ENC__1_2 = E2;
        last_ENC__1_3 = E3;
}

void pid_speed_1_motor_new(int32_t* PWM,
                         int32_t gate_left, int32_t gate_right, int32_t gate_cx){

    static int32_t SUM_pid_speed[10] = {0};
    static int32_t last_PWM = 0;

    const int register_num = 10;
    const float K = 100.0;
    const float lambda_p = 7 * K;
    const float lambda_i = 0.0 * K;
    const float lambda_d = 0.0 * K;

    const int MAX_PWM = 3000;

    int pwm;
    pwm = *PWM;
    
    int loss;
    loss = -(gate_cx-80)/(gate_left - gate_cx);

    int sum_loss;
    
    queue_push(SUM_pid_speed, loss, register_num);

    sum_loss = queue_sum(SUM_pid_speed, register_num);

    pwm = lambda_p*loss +lambda_i*sum_loss;

    int delta;
    delta = pwm - last_PWM;

    pwm = lambda_p*loss +lambda_i*sum_loss+lambda_d*delta;

    pwm = pwm>MAX_PWM?MAX_PWM:pwm;
    pwm = pwm<-MAX_PWM?-MAX_PWM:pwm;

    *PWM = pwm;
    last_PWM = pwm;
}

void pid_ball_area(int32_t* PWM1, int32_t* PWM3,
                   int32_t ball_area){

    static int32_t SUM_pid_speed_1[10] = {0};
    static int32_t SUM_pid_speed_3[10] = {0};
    static int32_t last_PWM_1 = 0;
    static int32_t last_PWM_3 = 0;

    const int register_num = 10;
    const float K = 100.0;
    const float lambda_p = 7 * K;
    const float lambda_i = 0.0 * K;
    const float lambda_d = 0.0 * K;

    const int goal_area = 3000;

    const int MAX_PWM = 1500;

    int pwm1, pwm3;
    pwm1 = *PWM1;
    pwm3 = *PWM3;

    int loss1, loss3;
    loss1 = goal_area - ball_area;
    loss3 = goal_area - ball_area;

    queue_push(SUM_pid_speed_1, loss1, register_num);
    queue_push(SUM_pid_speed_3, loss3, register_num);

    int sum_loss1, sum_loss3;
    sum_loss1 = queue_sum(SUM_pid_speed_1, register_num);
    sum_loss3 = queue_sum(SUM_pid_speed_3, register_num);

    pwm1 = lambda_p*loss1 +lambda_i*sum_loss1;
    pwm3 = lambda_p*loss3 +lambda_i*sum_loss3;

    pwm1 = pwm1>MAX_PWM?MAX_PWM:pwm1;
    pwm3 = pwm3>MAX_PWM?MAX_PWM:pwm3;
    pwm1 = pwm1<-MAX_PWM?-MAX_PWM:pwm1;
    pwm3 = pwm3<-MAX_PWM?-MAX_PWM:pwm3;

    *PWM1 = -pwm1;
    *PWM3 = pwm3;

    last_PWM_1 = pwm1;
    last_PWM_3 = pwm3;


}

void pid_fine_turn_ball(
    int32_t* PWM2, int32_t *PWM3,
    int32_t ball_x){
    
        static int32_t SUM_pid[50] = {0};
        static int32_t last_PWM = 0;

        const int register_num = 50;
        const float K = 0.5;
        const float lambda_p = 5 * K;
        const float lambda_i = 0.1 * K;
        const float lambda_d = 0.1 * K;
    
        const int MAX_PWM = 3000;
    
        int pwm;
        pwm = (int)((*PWM2+*PWM3)/2);
        
        int loss;
        loss = ball_x - 80;
    
        int sum_loss;
        
        queue_push(SUM_pid, loss, register_num);
    
        sum_loss = queue_sum(SUM_pid, register_num);

        pwm = lambda_p*loss +lambda_i*sum_loss;
    
        int delta; 
        delta = pwm - last_PWM;

        pwm = pwm + lambda_d*delta;
    
        pwm = pwm>MAX_PWM?MAX_PWM:pwm;
        pwm = pwm<-MAX_PWM?-MAX_PWM:pwm;
    
        *PWM2 = pwm;
        *PWM3 = pwm;

        last_PWM = pwm;
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
