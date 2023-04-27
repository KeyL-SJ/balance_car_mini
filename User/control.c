#include "control.h"

/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��	
��ڲ�������
����  ֵ����				 
**************************************************************************/
int HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    static u8 Flag_Target;                        //���ƺ�����ر������ṩ10ms��׼
    int Balance_Pwm, Velocity_Pwm;        //ƽ�⻷PWM�������ٶȻ�PWM����
    int Motor_Left, Motor_Right;      //���PWM���� Ӧ��Motor�� ��Moto�¾�
    if (GPIO_Pin == MPU6050_EXTI_Pin) {
        Flag_Target = !Flag_Target;
        Get_Angle();                            //������̬	5ms��ȡһ��
        Encoder_Left = -Read_Encoder(2);          //��ȡ���ֱ�������ֵ��ǰ��Ϊ��������Ϊ��
        Encoder_Right = -Read_Encoder(4);         //��ȡ���ֱ�������ֵ��ǰ��Ϊ��������Ϊ��
        //����A���TIM2_CH1,����A���TIM4_CH2,�����������������ļ�����ͬ
        if (Flag_Target == 1)                      //ʵ����10ms����һ��
            return 0;
        Key();                                  //ɨ�谴��״̬ ��������ͣ���

        Balance_Pwm = Balance(Angle_Balance, Gyro_Balance); //ƽ��PID����	  Gyro_Balanceƽ����ٶȼ��ԣ�ǰ��Ϊ��������Ϊ��
        Velocity_Pwm = Velocity(Encoder_Left, Encoder_Right);//�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ��

        Motor_Left = Balance_Pwm + Velocity_Pwm;              //�������ֵ������PWM
        Motor_Right = Balance_Pwm + Velocity_Pwm;             //�������ֵ������PWM

        Motor_Left = PWM_Limit(Motor_Left, 6900, -6900);
        Motor_Right = PWM_Limit(Motor_Right, 6900, -6900);        //PWM�޷�

        if (Turn_Off(Angle_Balance) == 0)                    //����������쳣
            Set_Pwm(Motor_Left, Motor_Right);                //��ֵ��PWM�Ĵ���
    }
    return 0;
}

/**************************************************************************
�������ܣ�ֱ��PD����		
��ڲ�����Angle:�Ƕȣ�Gyro�����ٶ�
����  ֵ����
**************************************************************************/
int Balance(float Angle, float Gyro) {
    float Balance_Kp = 300, Balance_Kd = 1.08;//ֱ����PD����
    float Angle_bias, Gyro_bias;
    int balance;
    Angle_bias = Middle_angle - Angle;                       //���ƽ��ĽǶ���ֵ �ͻ�е���
    Gyro_bias = 0 - Gyro;
    balance = -Balance_Kp * Angle_bias - Gyro_bias * Balance_Kd;        //����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ��
    return balance;
}

/**************************************************************************
�������ܣ��ٶȿ���PWM		
��ڲ�����encoder_left�����ֱ�����������encoder_right�����ֱ���������
����  ֵ���ٶȿ���PWM
**************************************************************************/
int Velocity(int encoder_left, int encoder_right) {
    float Velocity_Kp = 160, Velocity_Ki = 0.8;//�ٶȻ�PI����
    static float velocity, Encoder_Least, Encoder_bias;
    static float Encoder_Integral;
    //=============�ٶ�PI������=======================//
    Encoder_Least = 0 - (encoder_left + encoder_right);                    //��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩
    Encoder_bias *= 0.8;                                                  //һ�׵�ͨ�˲���
    Encoder_bias += Encoder_Least * 0.2;                                  //һ�׵�ͨ�˲���
    //�൱���ϴ�ƫ���0.8 + ����ƫ���0.2�������ٶȲ�ֵ�����ٶ�ֱ���ĸ���
    Encoder_Integral += Encoder_bias;                                  //���ֳ�λ�� ����ʱ�䣺10ms
    if (Encoder_Integral > 10000) Encoder_Integral = 10000;             //�����޷�
    if (Encoder_Integral < -10000) Encoder_Integral = -10000;            //�����޷�
    velocity = -Encoder_bias * Velocity_Kp - Encoder_Integral * Velocity_Ki;        //�ٶȿ���
    if (Turn_Off(Angle_Balance) == 1 || Flag_Stop == 1) Encoder_Integral = 0;      //����رպ��������
    return velocity;
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int motor_left, int motor_right) {
    if (motor_left > 0) BIN1 = 1, BIN2 = 0; //ǰ��
    else
        BIN1 = 0, BIN2 = 1; //����
    PWMB = myabs(motor_left);
    if (motor_right > 0) AIN2 = 1, AIN1 = 0;    //ǰ��
    else
        AIN2 = 0, AIN1 = 1; //����
    PWMA = myabs(motor_right);
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�����IN���������  max���޷����ֵ  min���޷���Сֵ
����  ֵ���޷����ֵ
**************************************************************************/
int PWM_Limit(int IN, int max, int min) {
    int OUT = IN;
    if (OUT > max) OUT = max;
    if (OUT < min) OUT = min;
    return OUT;
}

/**************************************************************************
�������ܣ������޸�С������״̬ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void) {
    u8 tmp;
    tmp = click();
    if (tmp == 1) Flag_Stop = !Flag_Stop;    //��������С������ͣ
}

/**************************************************************************
�������ܣ��쳣�رյ��		
��ڲ�����angle��С�����
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(float angle) {
    u8 temp;
    if (angle < -40 || angle > 40 || 1 == Flag_Stop)//��Ǵ���40�ȹرյ��
    {                                       //Flag_Stop��1�رյ��
        temp = 1;
        AIN1 = 0;
        AIN2 = 0;
        BIN1 = 0;
        BIN2 = 0;
    } else
        temp = 0;
    return temp;
}

/**************************************************************************
�������ܣ���ȡ�Ƕ�	
��ڲ�������
����  ֵ����
**************************************************************************/
void Get_Angle(void) {
    float Accel_Y, Accel_Z, Accel_X, Accel_Angle_x, Accel_Angle_y, Gyro_X, Gyro_Z, Gyro_Y;
    Gyro_X = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_XOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_XOUT_L);	//��ȡX��������
    Gyro_Y = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_YOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_YOUT_L);	//��ȡY��������
    Gyro_Z = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_ZOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_ZOUT_L);	//��ȡZ��������
    Accel_X = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_XOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_XOUT_L); //��ȡX����ٶȼ�
    Accel_Y = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_YOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_YOUT_L); //��ȡX����ٶȼ�
    Accel_Z = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_ZOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ�
    if (Gyro_X > 32768)
        Gyro_X -= 65536; //��������ת��  Ҳ��ͨ��shortǿ������ת��
    if (Gyro_Y > 32768)
        Gyro_Y -= 65536; //��������ת��  Ҳ��ͨ��shortǿ������ת��
    if (Gyro_Z > 32768)
        Gyro_Z -= 65536; //��������ת��
    if (Accel_X > 32768)
        Accel_X -= 65536; //��������ת��
    if (Accel_Y > 32768)
        Accel_Y -= 65536; //��������ת��
    if (Accel_Z > 32768)
        Accel_Z -= 65536;								//��������ת��
    Gyro_Balance = -Gyro_X;								//����ƽ����ٶ�
    Accel_Angle_x = atan2(Accel_Y, Accel_Z) * 180 / PI; //������ǣ�ת����λΪ��
    Accel_Angle_y = atan2(Accel_X, Accel_Z) * 180 / PI; //������ǣ�ת����λΪ��
    Gyro_X = Gyro_X / 16.4;								//����������ת�������̡�2000��/s��Ӧ������16.4���ɲ��ֲ�
    Gyro_Y = Gyro_Y / 16.4;								//����������ת��

    Pitch = -Kalman_Filter_x(Accel_Angle_x,Gyro_X);
    Roll = -Kalman_Filter_y(Accel_Angle_y,Gyro_Y);

    Angle_Balance = Pitch;	  //����ƽ�����
}

/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����a����Ҫ�������ֵ����
����  ֵ���޷�������
**************************************************************************/
int myabs(int a) {
    int temp;
    if (a < 0) temp = -a;
    else temp = a;
    return temp;
}
