/****************************************************************************************************************************************** 
* 文件名称:	SWM2X1_sleep.c
* 功能说明:	SWM2X1单片机的Sleep功能驱动库
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项:
* 版本日期:	V1.0.0		2016年1月30日
* 升级记录: 
*
*
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology 
*******************************************************************************************************************************************/
#include "SWM2X1.h"
#include "SWM2X1_sleep.h"


/* 注意：EnterSleepMode() 和 EnterStopMode() 必须在RAM中执行，Keil下实现方法有：
   方法一、Scatter file
   方法二、SWM2X1_sleep.c 上右键 =》Options for File "SWM2X1_sleep.c" =》Properties =》Memory Assignment =》Code/Conts 选择 IRAM1
*/


#if defined ( __ICCARM__ )
__ramfunc void EnterSleepMode(void)
#else
void EnterSleepMode(void)
#endif
{
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
	
	SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);
	
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
}


#if defined ( __ICCARM__ )
__ramfunc void EnterStopMode(void)
#else
void EnterStopMode(void)
#endif
{
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
	
	SYS->SLEEP |= (1 << SYS_SLEEP_STOP_Pos);
	
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
}
