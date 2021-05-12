//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\app_cfg.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void app_general_task(void)
{
    mde_pilot_light_task();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_int16u light_bt;
//------------------------------------------------------------------------------
void app_light_blink_task(void)
{
     macro_createTimer(timer_blink,timerType_second,0);
     
     pbc_timerClockRun_task(&timer_blink);
     if(pbc_pull_timerIsCompleted(&timer_blink))
     {
         pbc_reload_timerClock(&timer_blink,10);
         
         light_bt += 100;
         mde_push_pilot_light_ldms(0,light_bt,light_bt);
         
         if(light_bt > 1000)
         {
             light_bt = 0;
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