//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\snail_data_types.h"
#include "fm33lc0xx.h"
#include "intrinsics.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//名称:FM33LC025N时钟配置
//功能:配置晶振、FLASH等待时间、系统时钟
//HSE 12M,PLL 72M
//入口:无,在系统复位后,第一个调用
//出口:无
//-----------------------------------------------------------------------------
static void bsp_clock_cfg(void)
{
//    static sdt_int32u count=0;
//    
//    __disable_interrupt();
//    for(;count<50000;count++)
//    {
//        #ifdef NDEBUG
//        IWDG_ReloadKey();
//        #endif
//        __no_operation();  
//    }
//    RCC_DeInit();                        //系统时钟复位
//    RCC_ConfigHse(RCC_HSE_ENABLE);           //打开晶体振荡器
//    
//    for(;count<50000;count++)            //增加晶振起振时间，GD32
//    {
//        #ifdef NDEBUG
//        IWDG_ReloadKey();
//        #endif
//        __no_operation();  
//    }
//    if(ERROR==RCC_WaitHseStable())
//    {
//        while(1);                           //晶体出错,等待复位
//    }
//        
//    //FLASH->ACR |= FLASH_ACR_PRFTBE;                          //开启指令预取功能/* Enable Prefetch Buffer */
//    //FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);  //指令等待2周期 /* Flash 2 wait state */
//    //FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    
//    FLASH_SetLatency(FLASH_LATENCY_2);
//    
//    RCC_ConfigPll(RCC_PLL_SRC_HSE_DIV1,RCC_PLL_MUL_6);  //PLL配置
//
//    RCC_EnablePll(ENABLE);                                //打开PLL
//    while (RCC_GetFlagStatus(RCC_FLAG_PLLRD) == RESET)    //等待PLL稳定
//    {
//    }
//    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLLCLK);         //切换到PLL时钟
//    while (RCC_GetSysclkSrc() != 0x08)
//    {
//    }                                           //等待PLL成为系统时钟源
//    RCC_ConfigHclk(RCC_SYSCLK_DIV1);            //AHB时钟,
//    RCC_ConfigPclk1(RCC_HCLK_DIV2);             //APB1时钟,AHB时钟2分频
//    RCC_ConfigPclk2(RCC_HCLK_DIV1);             //APB2时钟,
//    
//    IntVectorConfigure();

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_int16u tick_millsecond;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//系统时钟中断，1ms
//-----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    tick_millsecond++;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++interface++++++++++++++++++++++++++++++++++
sdt_int16u bsp_pullSystemTickCount(void)
{
    return(tick_millsecond);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//名称:系统滴答时钟配置
//功能:滴答时钟源选择,中断开启
//SysTick_Config函数,采用AHB时钟源
//1ms的定时
//-----------------------------------------------------------------------------
void bsp_system_tick_cfg(void)
{
    bsp_clock_cfg();
    SysTick_Config(72000);
    __enable_interrupt();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
