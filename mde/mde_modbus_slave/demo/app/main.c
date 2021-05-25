//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\app_cfg.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void app_general_task(void)
{
  
    mde_log_message_task();
    
    macro_createTimer(timer_logtx,timerType_millisecond,0);
    
    pbc_timerClockRun_task(&timer_logtx);
    if(pbc_pull_timerIsCompleted(&timer_logtx))
    {
        pbc_reload_timerClock(&timer_logtx,2000);
        //mde_push_log_character_once("LOG MESSAGE DEMO");
    } 
  
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_int16u demo_wreg[256];
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:读寄存器数据,mde_modbus_slave 回调函数
//fun:
//in: in_reg_addr 寄存器地址
//    in_reg_length 寄存器长度
//    out_pWords  读取的内容指针,
//out: sdt_true 读取完成，sdt_false 有错误发生
//------------------------------------------------------------------------------
sdt_bool demo_read_modbus_slave_register(sdt_int16u in_reg_addr,sdt_int8u in_reg_length,sdt_int16u* out_pWords)
{
    if(in_reg_addr > 1000)  //地址超范围,出错
    {
        return(sdt_false);
    }
    while(in_reg_length)
    {
        switch(in_reg_addr)
        {
           case 0:
           {
               *out_pWords = demo_wreg[0] + 1;
               break;
           }      
           case 1:
           {
               *out_pWords = demo_wreg[1] + 1;
               break;
           }   
           case 2:
           {
               *out_pWords = demo_wreg[2] + 1;
               break;
           }   
           default:
           {
               break;
           }
        }
        out_pWords ++;
        in_reg_addr ++;
        in_reg_length --;
    }
    return(sdt_true);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:写寄存器数据,mde_modbus_slave 回调函数
//fun:
//in:  in_reg_addr   寄存器地址
//     in_reg_length 寄存器长度
//     in_pWords     写入的内容指针,
//out: sdt_true 读取完成，sdt_false 有错误发生
//------------------------------------------------------------------------------
sdt_bool demo_write_modbus_slave_register(sdt_int16u in_reg_addr,sdt_int8u in_reg_length,sdt_int16u* in_pWords)
{
    if(in_reg_addr > 1000)  //地址超范围,出错
    {
        return(sdt_false);
    }
    while(in_reg_length)
    {
        switch(in_reg_addr)
        {
           case 0:
           {
               demo_wreg[0] = *in_pWords;
               break;
           }      
           case 1:
           {
               demo_wreg[1] = *in_pWords;
               break;
           }   
           case 2:
           {
               demo_wreg[2] = *in_pWords;
               break;
           }   
           default:
           {
               break;
           }
        }
        in_pWords ++;
        in_reg_addr ++;
        in_reg_length --;
    }
    return(sdt_true);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define modbus_demo_solid  0
//------------------------------------------------------------------------------
//示例任务
//------------------------------------------------------------------------------
void app_modbus_slave_demo_task(void)
{
    static sdt_bool cfged = sdt_false;
    
    if(cfged)
    {
        mde_modbus_slave_task();
    }
    else
    {
        mdsRtu_parameter_def cfg_mdsRtu_parameter;
        
        cfg_mdsRtu_parameter.mdsRtu_address = 1;
        cfg_mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_baudrate = 9600;
        cfg_mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_sysFrequency = 72000000; //系统频率，用于bsp的波特率计算
        cfg_mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_parity = mdsRtu_parity_none;
        cfg_mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_stopBits = mdsRtu_stopBits_one;
        cfg_mdsRtu_parameter.cbk_read_modbus_slave_register = demo_read_modbus_slave_register;
        cfg_mdsRtu_parameter.cbk_write_modbus_slave_register = demo_write_modbus_slave_register;
        
        mde_push_modbus_slave_parameter(modbus_demo_solid,&cfg_mdsRtu_parameter);
        cfged = sdt_true;
    }  
//------------------------------------------------------------------------------
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void main(void)
{
    pbc_task_create(app_general_task,0);
    pbc_task_create(app_modbus_slave_demo_task,0);  
//------------------------------------------------------------------------------
    pbc_task_process();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++