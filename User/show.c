#include "show.h"

/**************************************************************************
�������ܣ�OLED��ʾ
��ڲ�������
����  ֵ����
**************************************************************************/
void oled_show(void) {
    OLED_Display_On();  //��ʾ����
    //=============��һ����ʾС��ģʽ=======================//
    OLED_ShowString(0, 0, "Kalman");
    OLED_ShowString(60, 0, "Balance");
    //=============��������ʾ������1=======================//
    OLED_ShowString(00, 20, "EncoderL");
    if (Encoder_Left < 0)
        OLED_ShowString(80, 20, "-"),
                OLED_ShowNumber(95, 20, -Encoder_Left, 3, 12);
    else
        OLED_ShowString(80, 20, "+"),
                OLED_ShowNumber(95, 20, Encoder_Left, 3, 12);
    //=============��������ʾ������2=======================//
    OLED_ShowString(00, 30, "EncoderR");
    if (Encoder_Right < 0)
        OLED_ShowString(80, 30, "-"),
                OLED_ShowNumber(95, 30, -Encoder_Right, 3, 12);
    else
        OLED_ShowString(80, 30, "+"),
                OLED_ShowNumber(95, 30, Encoder_Right, 3, 12);
    //=============��������ʾ�Ƕ��뿪��=======================//
    OLED_ShowString(0, 50, "Angle");
    if (Angle_Balance < 0) OLED_ShowString(48, 50, "-");
    if (Angle_Balance >= 0) OLED_ShowString(48, 50, "+");
    OLED_ShowNumber(53, 50, myabs((int) Angle_Balance), 3, 12);
    if (Flag_Stop) OLED_ShowString(90, 50, "OFF");
    if (!Flag_Stop) OLED_ShowString(90, 50, "ON ");
    //=============ˢ��=======================//
    OLED_Refresh_Gram();
}
