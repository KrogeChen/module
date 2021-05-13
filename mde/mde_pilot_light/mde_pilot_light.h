#ifndef mde_pilot_light_H
#define mde_pilot_light_H
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef snail_data_types
    #include ".\depend\snail_data_types.h"
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++     

//++++++++++++++++++++++++++++++++++task++++++++++++++++++++++++++++++++++++++++
//名称: pilot light模块任务
//功能: 
//入口: 无
//
//出口: 无
//------------------------------------------------------------------------------
void mde_pilot_light_task(void);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++interface+++++++++++++++++++++++++++++++++++++
//name:设置指示灯亮、暗时间
//fun:
//in:   in_solid_number          实列号
//      in_ms_lighten           点亮的时间
//      in_ms_dark              熄灭的时间,
//out:  none
//------------------------------------------------------------------------------
void mde_push_pilot_light_ldms(sdt_int8u in_solid_number,sdt_int16u in_ms_lighten,sdt_int16u in_ms_dark);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++