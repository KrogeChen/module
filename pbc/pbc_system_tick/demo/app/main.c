//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\app_cfg.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void app_general_task(void)
{
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_bool light = sdt_false;
//------------------------------------------------------------------------------
void app_light_blink_task(void)
{
     macro_createTimer(timer_onoff,timerType_second,0);
     
     pbc_timerClockRun_task(&timer_onoff);
     if(pbc_pull_timerIsCompleted(&timer_onoff))
     {
         pbc_reload_timerClock(&timer_onoff,5);
         
         if(light)
         {
             light = sdt_false;
         }
         else
         {
             light = sdt_true;
         }
     }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void main(void)
{
    pbc_task_create(app_general_task,0);
    pbc_task_create(app_light_blink_task,100);  
//------------------------------------------------------------------------------
    pbc_task_process();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++