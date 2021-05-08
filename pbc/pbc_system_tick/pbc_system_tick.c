//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//version:10
//date: 06/11/2019
//------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\depend\snail_data_types.h"
#include ".\depend\bsp_system_tick.h"
#include ".\pbc_system_tick.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define taskTotal      8   
//------------------------------------------------------------------------------
typedef struct
{
    sdt_bool       taskRun;         //运行标志
    sdt_int16u     timer;           //任务时间计数
    sdt_int16u     timerPeriod;     //任务周期
    pTaskFun_def   taskHook;          //任务函数指针
}taskSCH_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
taskSCH_def taskTable[taskTotal];
static sdt_bool system_cfged = sdt_false;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_int16u sysTickMillsecond;
static sdt_int16u sysTickSecond;
static sdt_int16u sysTickMinute;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void system_clock_cfg(void)
{
    sdt_int8u i;
    for(i = 0;i < taskTotal;i++) //期初，清理一次指针
    {
        taskTable[i].taskHook = 0;
    }  
    bsp_system_tick_cfg();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_int16u pbc_getSysTickMillisecond(void)
{
    sdt_int16u ReadData = 0;
    do
    {
        ReadData=sysTickMillsecond;
    }while(ReadData!=sysTickMillsecond);
    return(ReadData);
}

static void pbc_sysTick_run(void)
{
    static sdt_int16u oneSecondCnt;
    static sdt_int8u oneMinuteCnt;
    static sdt_int16u tickBackup;
    sdt_int16u currentTick;
    sdt_int16u goRunTick;
    
    currentTick = bsp_pullSystemTickCount();
    goRunTick = currentTick - tickBackup;
    tickBackup = currentTick;
    while(goRunTick)
    {
        goRunTick--;
        sysTickMillsecond++;
        oneSecondCnt++;
        if(oneSecondCnt > 999)
        {
            oneSecondCnt = 0;
            sysTickSecond++;
            oneMinuteCnt++;
            if(oneMinuteCnt > 59)
            {
                oneMinuteCnt = 0;
                sysTickMinute++;
            }
        }
//------------------------------------------------------------------------------
        sdt_int8u i;
        
        for(i = 0;i < taskTotal;i++)
        {
            if(0 == taskTable[i].taskHook)
            {
            }
            else
            {
                if(0 != taskTable[i].timer)
                {
                    taskTable[i].timer--;
                }
                if(0 == taskTable[i].timer)
                {
                    taskTable[i].taskRun = sdt_true;
                    taskTable[i].timer = taskTable[i].timerPeriod;
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++interface+++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
void pbc_task_process(void)
{
    while(1)
    {
        if(system_cfged)
        {
            pbc_sysTick_run();
//------------------------------------------------------------------------------
            sdt_int8u i;
            
            for(i = 0;i < taskTotal;i++)
            {
                if(0 == taskTable[i].taskHook)
                {
                }
                else if(taskTable[i].taskRun)
                {
                    taskTable[i].taskHook();
                    taskTable[i].taskRun = sdt_false;
                }
            }
        }
        else
        {
            system_cfged = sdt_true;
            system_clock_cfg();
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void pbc_timerClockRun_task(timerClock_def* in_timer)
{
    sdt_int16u currentTick;
    sdt_int16u goRunTick;
    sdt_int8u timer_type;
    
    pbc_sysTick_run();
    timer_type = in_timer->timStatusBits & timStatusBits_typeMask;
    if(timStatusBits_typeMillsecond == timer_type)
    {
        currentTick = sysTickMillsecond;
    }
    else if(timStatusBits_typeSecond == timer_type)
    {
        currentTick = sysTickSecond;
    }
    else if(timStatusBits_typeMinute == timer_type)
    {
        currentTick = sysTickMinute;
    }
    else
    {
        while(1);
    }
    goRunTick = currentTick - in_timer->keepTick;
    in_timer->keepTick = currentTick;
    
    if(timStatusBits_typeMillsecond == timer_type)
    {
        if(goRunTick > 2000)
        {
            in_timer->timClock = 0;
        }
    }
    else if(timStatusBits_typeSecond == timer_type)
    {
        if(goRunTick > 3)
        {
            in_timer->timClock = 0;
        }
    }
    else if(timStatusBits_typeMinute == timer_type)
    {
        if(goRunTick > 2)
        {
            in_timer->timClock = 0;
        }
    }
    
    while(goRunTick)
    {
        if(0 != in_timer->timClock)
        {
            in_timer->timClock--;
            if(0 == in_timer->timClock)
            {
                return;
            } 
        }
        else
        {
            return;
        }
        goRunTick--;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void pbc_timerMillRun_task(timerClock_def* in_timer)
{
    in_timer->timStatusBits &= ~timStatusBits_typeMask;  //0 is millsecond type
    //in_timer->timStatusBits |= timStatusBits_typeMillsecond;
    pbc_timerClockRun_task(in_timer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-------------------------------------------------------------------------------------------------
sdt_bool pbc_pull_timerIsCompleted(timerClock_def* in_timer)
{
    pbc_timerClockRun_task(in_timer);
    if(0 == in_timer->timClock)
    {
        return(sdt_true);
    }
    else
    {
        return(sdt_false);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-------------------------------------------------------------------------------------------------
sdt_bool pbc_pull_timerIsOnceTriggered(timerClock_def* in_timer)
{
    pbc_timerClockRun_task(in_timer);
    if(in_timer->timStatusBits & timStatusBits_onceTriggered)
    {
    }
    else
    {
        if(0 == in_timer->timClock)
        {
            in_timer->timStatusBits |= timStatusBits_onceTriggered;
            return(sdt_true);
        }
    }
    return(sdt_false);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-------------------------------------------------------------------------------------------------
void pbc_stop_timerIsOnceTriggered(timerClock_def* in_timer)
{
    in_timer->timStatusBits |= timStatusBits_onceTriggered;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void pbc_reload_timerClock(timerClock_def* in_timer,sdt_int16u in_value)
{
    pbc_timerClockRun_task(in_timer);
    in_timer->timClock = in_value;
    in_timer->timStatusBits &= (~timStatusBits_onceTriggered);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
sdt_int8u pbc_task_create(pTaskFun_def in_taskPointer,sdt_int16u sch_period)
{
    sdt_int8u i;

    if(system_cfged)
    {
    }
    else
    {
        system_cfged = sdt_true;
        system_clock_cfg();
    }
    for(i = 0;i < taskTotal;i++)
    {
        if(0 == taskTable[i].taskHook)
        {
            taskTable[i].taskRun = sdt_false;
            taskTable[i].taskHook = in_taskPointer;
            taskTable[i].timer = 0;
            taskTable[i].timerPeriod = sch_period;
            return(i);
        }
    }
    return(err_createTaskFail);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
void pbc_taskImmediatelyRun(pTaskFun_def in_taskPointer)
{
    in_taskPointer();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++