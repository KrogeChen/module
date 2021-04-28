//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:log链路代码，日常数据模块
//version:10
//date: 04/27/2021
//------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "..\..\pbc\pbc_system_tick\pbc_system_tick.h"
//------------------------------------------------------------------------------
#include ".\depend\bsp_log_message.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define OMS_CMPS           10  //一个报文接收时间
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define max_rxd_len        128
#define max_txd_len        128
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef enum
{
    log_rs_idle            = 0x00,
    log_rs_rx_wait         = 0x01,
    log_rs_rx_doing        = 0x02,
    log_rs_rx_complete     = 0x03,
    
    log_rs_tx_start        = 0x10,
    log_rs_tx_doing        = 0x11,
    log_rs_tx_complete     = 0x12,
}log_run_status_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define log_bits_message_rx     0x0001 //新报文生产
#define log_bits_rx_byte        0x0002 //收一字节数据
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
typedef struct
{
    sdt_int8u             rx_byte;
    sdt_int16u            log_bits;  //状态字
    log_run_status_def    log_run_status;
    
    sdt_int8u             rx_buff[max_rxd_len];
    sdt_int16u            receive_len;
    sdt_int16u            receive_index;
    sdt_int8u             tx_buff[max_txd_len];
    sdt_int16u            transmit_len;
    sdt_int16u            transmit_index;
    
    timerClock_def        timer_rxoms;   //完成一次接收消息

    
    void (*pull_look_for_byte_rx)(void);
    sdt_int16u (*transfet_bytes_tx_phy)(sdt_int8u* in_pByte,sdt_int16u in_expect_bytes);
    sdt_bool (*pull_complete_tx_data)(void);
    void (*entry_phy_rx)(void);
    void (*entry_phy_tx)(void);
}log_oper_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//soild 
#include ".\solid_log_message.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//日常状态跳变
//------------------------------------------------------------------------------
static void log_link_status_jump(log_oper_def* mix_pLog_oper)
{
    log_run_status_def read_log_run_status;
    
    /*
    pbc_timerClockRun_task(&mix_pMbslk_oper->timer_brate_cal);
    
    pbc_timerMillRun_task(&mix_pMbslk_oper->timer_rx_timeout);
    pbc_timerMillRun_task(&mix_pMbslk_oper->timer_tx_timeout);
    
    if(pbc_pull_timerIsOnceTriggered(&mix_pMbslk_oper->timer_rx_timeout))
    {
        mix_pMbslk_oper->mbslk_run_status = mbslk_rs_idle;
    }    
    if(pbc_pull_timerIsOnceTriggered(&mix_pMbslk_oper->timer_tx_timeout))
    {
        mix_pMbslk_oper->mbslk_run_status = mbslk_rs_idle;
        mix_pMbslk_oper->transmit_monitor = mbk_trans_mon_error;
    }*/
    
    do
    {
        read_log_run_status = mix_pLog_oper->log_run_status;
        switch(read_log_run_status)
        {
            case log_rs_idle:
            {
                mix_pLog_oper->log_run_status = log_rs_rx_wait;
                break;
            }
            case log_rs_rx_wait:
            {
                
                if(log_bits_rx_byte & mix_pLog_oper->log_bits)
                {
                    mix_pLog_oper->log_bits &= ~log_bits_rx_byte;
                    mix_pLog_oper->rx_buff[0] = mix_pLog_oper->rx_byte;
                    mix_pLog_oper->receive_len = 1;
                    pbc_reload_timerClock(&mix_pLog_oper->timer_rxoms,OMS_CMPS);
                    mix_pLog_oper->log_run_status = log_rs_rx_doing;
                }
                break;
            }
            case log_rs_rx_doing:
            {
                if(log_bits_rx_byte & mix_pLog_oper->log_bits)
                {
                    mix_pLog_oper->log_bits &= ~log_bits_rx_byte;
                    mix_pLog_oper->rx_buff[mix_pLog_oper->receive_len] = mix_pLog_oper->rx_byte;
                    mix_pLog_oper->receive_len ++;
                    pbc_reload_timerClock(&mix_pLog_oper->timer_rxoms,OMS_CMPS);
                }
                else
                {
                    if(pbc_pull_timerIsCompleted(&mix_pLog_oper->timer_rxoms))
                    {
                        mix_pLog_oper->log_run_status = log_rs_rx_complete;
                    }  
                }
                break;
            }
            case log_rs_rx_complete:
            {
                mix_pLog_oper->log_bits |= log_bits_message_rx;
                mix_pLog_oper->log_run_status = log_rs_rx_wait;
                break;
            }
            case log_rs_tx_start:
            {
                sdt_int16u surplus_bytes;
                
                mix_pLog_oper->transmit_index = 0;
                surplus_bytes = mix_pLog_oper->transfet_bytes_tx_phy(&mix_pLog_oper->tx_buff[0],(mix_pLog_oper->transmit_len - mix_pLog_oper->transmit_index));
                mix_pLog_oper->transmit_index = mix_pLog_oper->transmit_len - surplus_bytes;
                mix_pLog_oper->log_run_status = log_rs_tx_doing;
                break;
            }
            case log_rs_tx_doing:
            {
                sdt_int16u surplus_bytes;
                
                if(mix_pLog_oper->transmit_index == mix_pLog_oper->transmit_len)  //数据转移完毕
                {
                    mix_pLog_oper->log_run_status = log_rs_tx_complete;
                }
                else
                {
                    surplus_bytes = mix_pLog_oper->transfet_bytes_tx_phy(&mix_pLog_oper->tx_buff[0],(mix_pLog_oper->transmit_len - mix_pLog_oper->transmit_index));
                    mix_pLog_oper->transmit_index = mix_pLog_oper->transmit_len - surplus_bytes;
                }
                break;
            }
            
            case log_rs_tx_complete:
            {
                if(mix_pLog_oper->pull_complete_tx_data())
                {
                    mix_pLog_oper->log_run_status = log_rs_rx_wait;
                }
                
                pbc_stop_timerIsOnceTriggered(&mix_pMbslk_oper->timer_tx_timeout);
                break;
            }
            default:
            {
                mix_pLog_oper->log_run_status = log_rs_idle;
                break;
            }
        }
        
    }while(read_log_run_status != mix_pLog_oper->log_run_status);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//独立模块任务
//------------------------------------------------------------------------------
static void alone_log_link_task(log_oper_def* mix_pLog_oper)
{
    mix_pLog_oper->pull_look_for_byte_rx();
    mbus_link_status_jump(mix_pLog_oper);
}
//++++++++++++++++++++++++++++++++++++interface+++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
void mde_log_message_task(void)
{
    static sdt_bool cfged = sdt_false;

    if(cfged)
    {
        sdt_int8u i;
        for(i = 0;i < max_solid;i ++)
        {
            alone_log_link_task(&log_oper_solid[i]);
        }
    }
    else
    {
        cfged = sdt_true;
        log_solid_cfg();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:推送文本日志到log模块
//in:   in_pCharacter -- 字符指针
//out:  sdt_true: 推送成功
//------------------------------------------------------------------------------
sdt_bool mde_push_log_character_once(sdt_int8s* in_pCharacter)
{
    sdt_int16u i;
    
    if(0x00 == in_pCharacter[0]) //无字符直接退出
    {
        return(sdt_true);
    }
    else if(log_rs_rx_wait == log_oper_solid[0].log_run_status)
    {
        for(i = 0;i < max_txd_len;i++)
        {
            log_oper_solid[0].tx_buff[i] = in_pCharacter[i];
            if(0 == in_pCharacter[i+1])
            {
                break;
            }
        }
        log_oper_solid[0].transmit_len = i+1;
        log_oper_solid[0].log_run_status = log_rs_tx_start;
        log_link_status_jump(&log_oper_solid[0]);
        return(sdt_true);
    }
    else
    {
        alone_log_link_task(&log_oper_solid[0]); //处理任务模块
        return(sdt_false);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:推入一字节的接收数据到链路,一般由bsp层调用
//in:   
//      in_rx_byte      -- 接收的数据
//out:  none
//------------------------------------------------------------------------------
void mde_push_log_one_receive_byte(sdt_int8u in_rx_byte)
{
    log_oper_solid[0].log_bits |= log_bits_rx_byte;
    log_oper_solid[0].rx_byte = in_rx_byte;
    log_link_status_jump(&log_oper_solid[0]);//数据进入后,马上处理状态
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++