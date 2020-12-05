//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\snail_data_types.h"
#include "sysctrl.h"
#include "intrinsics.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_int16u tick_millsecond;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_int16u bsp_pullSystemTickCount(void)
{
    return(tick_millsecond);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//名称:系统滴答时钟配置
//功能:滴答时钟源选择,中断开启
//SysTick_Config函数,采用AHB时钟源
//1ms的定时
//-------------------------------------------------------------------------------------------------

void bsp_system_tick_cfg(void)
{
    SysTick_Config(24000);
    __enable_interrupt();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//系统时钟中断，1ms
//-------------------------------------------------------------------------------------------------
void SysTick_Handler(void)
{
    tick_millsecond++;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++