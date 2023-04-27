/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：5.7
修改时间：2021-04-29

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:5.7
Update：2021-04-29

All rights reserved
***********************************************/
#include "key.h"

/**************************************************************************
Function: Key scan
Input   : none
Output  : 0：No action；1：click；
函数功能：按键扫描
入口参数：无
返回  值：按键状态 0：无动作 1：单击 
**************************************************************************/
u8 click(void)
{
		static u8 flag_key=1;//按键松开标志
		if(flag_key&&KEY==0) //检测到按键按下
		{
			flag_key=0;
			return 1;	//按键按下
		}
		else if(1==KEY)			flag_key=1;
		return 0;//无按键按下
}
