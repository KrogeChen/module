//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\snail_data_types.h"
#include "sysctrl.h"
#include "intrinsics.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------*/
#define RC_TRIM_BASE_ADDR           ((volatile uint16_t*)   (0x00100C00ul))
#define RCH_CR_TRIM_24M_VAL         (*((volatile uint16_t*) (0x00100C00ul)))
#define RCH_CR_TRIM_22_12M_VAL      (*((volatile uint16_t*) (0x00100C02ul)))
#define RCH_CR_TRIM_16M_VAL         (*((volatile uint16_t*) (0x00100C04ul)))
#define RCH_CR_TRIM_8M_VAL          (*((volatile uint16_t*) (0x00100C06ul)))
#define RCH_CR_TRIM_4M_VAL          (*((volatile uint16_t*) (0x00100C08ul)))

#define RCL_CR_TRIM_38400_VAL       (*((volatile uint16_t*) (0x00100C20ul)))
#define RCL_CR_TRIM_32768_VAL       (*((volatile uint16_t*) (0x00100C22ul)))
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//名称:HC32L136时钟配置
//功能:配置晶振、FLASH等待时间、系统时钟
//RCH 24M
//入口:无,在系统复位后,第一个调用
//出口:无
//-----------------------------------------------------------------------------
static void bsp_clock_cfg(void)
{
    #ifdef NDEBUG
    *(volatile uint32_t*)0xE000ED08 = 0x00000800;  //app_vect_table
    #endif
    M0P_SYSCTRL->PERI_CLKEN_f.FLASH = 1;
    //时钟切换到RCL                                    
    M0P_SYSCTRL->RCL_CR_f.TRIM = RCL_CR_TRIM_38400_VAL;
    M0P_SYSCTRL->RCL_CR_f.STARTUP = 0x01;   //16个周期
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;         //unlock
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.RCL_EN = 1;
    while(1 != M0P_SYSCTRL->RCL_CR_f.STABLE)
    {
    }
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;         //unlock
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.CLKSW = SysctrlClkRCL;
    
    M0P_FLASH->BYPASS = 0x5A5A;
    M0P_FLASH->BYPASS = 0xA5A5;
    M0P_FLASH->CR_f.WAIT = 0;  //flash 等待时间
    
    //时钟切换到RCH
    M0P_SYSCTRL->RCH_CR_f.TRIM = RCH_CR_TRIM_24M_VAL;  //24M
    if(1 != M0P_SYSCTRL->SYSCTRL0_f.RCH_EN)
    {
        M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;         //unlock
        M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
        M0P_SYSCTRL->SYSCTRL0_f.RCH_EN = 1;
    }
    while(1 != M0P_SYSCTRL->RCH_CR_f.STABLE)
    {
    }
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;         //unlock
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.CLKSW = SysctrlClkRCH;
   // Flash_WaitCycle(FlashWaitCycle0);       //flash 等待时间
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;         //unlock
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.RCL_EN = 0;     //关闭RCL
    
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;         //unlock
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.HCLK_PRS = SysctrlHclkDiv1;  //HCLK 
      
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;         //unlock
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.PCLK_PRS = SysctrlPclkDiv1;  //PCLK

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
    bsp_clock_cfg();
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