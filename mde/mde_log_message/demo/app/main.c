//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\app_cfg.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void app_general_task(void)
{
    mde_mbus_link_task();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define mbus_demo_smb       0 
//------------------------------------------------------------------------------
static sdt_bool light = sdt_false;
//------------------------------------------------------------------------------
void app_mbus_demo_task(void)
{
    if(mde_pull_mbus_recevie_one_message(mbus_demo_smb))//收到一个报文
    {
        mbus_link_buff_def* link_pBuff;
        
        link_pBuff = mde_pull_mbus_message_pBuff(mbus_demo_smb); //获取报文内容
        
        link_pBuff->meter_addr[0] = 0x12;//改变收发缓冲区内容
        link_pBuff->meter_addr[1] = 0x34;
        link_pBuff->meter_addr[2] = 0x56;
        link_pBuff->meter_addr[3] = 0x78;
        link_pBuff->meter_addr[4] = 0x90;
        link_pBuff->meter_addr[5] = 0x55;
        link_pBuff->meter_addr[6] = 0x55;
        link_pBuff->control_code ++;                             
        link_pBuff->payload[0] ++;
        link_pBuff->payload[1] ++;
        link_pBuff->payload[2] ++;
        link_pBuff->payload[3] ++;
        link_pBuff->payload[4] ++;
        link_pBuff->payload[5] ++;
        link_pBuff->payload[6] ++;
        link_pBuff->payload[7] ++;
        link_pBuff->payload[8] ++;
        
        mde_push_mbus_active_one_message_transmit(mbus_demo_smb); //激活发送
    }
    
    if(mbk_trans_mon_complete == mde_pull_mbus_transmit_monitor(mbus_demo_smb))
    {
    }
    else if(mbk_trans_mon_error == mde_pull_mbus_transmit_monitor(mbus_demo_smb))
    {
    }
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void main(void)
{
    pbc_task_create(app_general_task,0);
    pbc_task_create(app_mbus_demo_task,0);  
//------------------------------------------------------------------------------
    pbc_task_process();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++