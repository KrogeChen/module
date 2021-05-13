//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:modbus 从机代码 RTU模式
//version:10
//date: 05/07/2021
//------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "..\..\pbc\pbc_system_tick\pbc_system_tick.h"
//------------------------------------------------------------------------------
#include ".\depend\bsp_pilot_light.h"
//------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
typedef struct
{
    sdt_int8u                lamp_id;
    sdt_bool                 light_lighten;
    
    sdt_int16u               lighten_ms;
    sdt_int16u               dark_ms;
    
    timerClock_def           timer_dark;     //熄灭定时器
    timerClock_def           timer_lighten;  //点亮定时器

}pilot_oper_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//solid 
#include ".\solid_pilot_light.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//独立模块任务
//------------------------------------------------------------------------------
static void alone_pilot_light_task(pilot_oper_def* mix_pPilot_oper)
{
    pbc_timerMillRun_task(&mix_pPilot_oper->timer_dark);
    pbc_timerMillRun_task(&mix_pPilot_oper->timer_lighten);

    if(0 == mix_pPilot_oper->dark_ms)
    {
        mix_pPilot_oper->light_lighten = sdt_true;
    }    
    else if(0 == mix_pPilot_oper->lighten_ms)
    {
        mix_pPilot_oper->light_lighten = sdt_false;
    }
    else
    {
        if(mix_pPilot_oper->light_lighten)
        {
            if(pbc_pull_timerIsCompleted(&mix_pPilot_oper->timer_lighten))
            {
                mix_pPilot_oper->light_lighten = sdt_false;
                pbc_reload_timerClock(&mix_pPilot_oper->timer_dark,mix_pPilot_oper->lighten_ms);
            } 
        }
        else
        {
            if(pbc_pull_timerIsCompleted(&mix_pPilot_oper->timer_dark))
            {
                mix_pPilot_oper->light_lighten = sdt_true;
                pbc_reload_timerClock(&mix_pPilot_oper->timer_lighten,mix_pPilot_oper->lighten_ms);
            } 
           
        }
    }
    if(mix_pPilot_oper->light_lighten)
    {
        bsp_pilot_light_lighten(mix_pPilot_oper->lamp_id);
    }
    else
    {
        bsp_pilot_light_dark(mix_pPilot_oper->lamp_id);
    }
}
//++++++++++++++++++++++++++++++++++++interface+++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
void mde_pilot_light_task(void)
{
    static sdt_bool cfged = sdt_false;
    
    if(cfged)
    {
        sdt_int8u i;
        for(i = 0;i < max_solid;i ++)
        {
            alone_pilot_light_task(&pilot_oper_solid[i]);
        }
    }
    else
    {
        cfged = sdt_true;
        pilot_solid_cfg();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:设置指示灯亮、暗时间
//fun:
//in:   in_solid_number          实列号
//      in_ms_lighten           点亮的时间
//      in_ms_dark              熄灭的时间,
//out:  none
//------------------------------------------------------------------------------
void mde_push_pilot_light_ldms(sdt_int8u in_solid_number,sdt_int16u in_ms_lighten,sdt_int16u in_ms_dark)
{
    if(in_solid_number < max_solid)
    {
        pilot_oper_solid[in_solid_number].lighten_ms = in_ms_lighten;
        pilot_oper_solid[in_solid_number].dark_ms = in_ms_dark;
    }
    else
    {
        while(1);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++