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
#define max_reg_len        128
#define max_rxd_len        128
#define max_txd_len        128
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef enum
{
    mdsRtu_rs_idle            = 0x00,
    mdsRtu_rs_rx_wait         = 0x01,
    mdsRtu_rs_rx_doing        = 0x02,
    mdsRtu_rs_rx_complete     = 0x03,

    mdsRtu_rs_tx_start        = 0x10,
    mdsRtu_rs_tx_35t          = 0x11,
    mdsRtu_rs_tx_doing        = 0x12,
    mdsRtu_rs_tx_stop         = 0x13,
    mdsRtu_rs_tx_complete     = 0x14,
}mdsRtu_run_status_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define mdsRtu_bits_message_rx     0x0001 //新报文生产
#define mdsRtu_bits_rx_byte        0x0002 //收一字节数据
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//------------------------------------------------------------------------------
typedef struct
{
    mdsRtu_parameter_def     mdsRtu_parameter;
    sdt_int8u                rx_byte;
    sdt_int16u               mdsRtu_bits;  //状态字
    mdsRtu_run_status_def    mdsRtu_run_status;
    
    sdt_int16u               reg_details[max_reg_len];
    sdt_int8u                rx_buff[max_rxd_len];
    sdt_int16u               receive_len;
    sdt_int8u                tx_buff[max_txd_len];
    sdt_int16u               transmit_len;
    sdt_int16u               transmit_index;
    sdt_int16u               timer_busfree_cnt;
    
    timerClock_def           timer_rxoms;   //完成一次接收消息

    

    
    void (*pull_look_for_byte_rx)(void);
    void (*phy_reload)(void);
    
    sdt_int16u (*transfet_bytes_tx_phy)(sdt_int8u* in_pByte,sdt_int16u in_expect_bytes);
    sdt_bool (*pull_complete_tx_data)(void);
    void (*entry_phy_rx)(void);
    void (*entry_phy_tx)(void);
}mdsRtu_oper_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//solid 
#include ".\solid_modbus_slave.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//modbus 从机协议解释
//out: sdt_true 协议处理成功 应答，sdt_false 协议处理失败
//------------------------------------------------------------------------------
static sdt_bool modbus_slave_protocol(mdsRtu_oper_def* mix_pMdsRtu_oper)
{
    sdt_int8u rd_lenght;
    sdt_int16u rx_readReg_addr;
    sdt_int8u  rx_readReg_len;
    sdt_int16u rx_writeReg_addr;
    sdt_int8u  rx_writeReg_len;
    
    rd_lenght = mix_pMdsRtu_oper->receive_len;
    
    if(rd_lenght < 6)
    {
        return(sdt_false);
    }
    if((mix_pMdsRtu_oper->.mdsRtu_parameter.mdsRtu_address == mix_pMdsRtu_oper->rx_buff[0]) || (0xFE == mix_pMdsRtu_oper->rx_buff[0]) || (0xFA == mix_pMdsRtu_oper->rx_buff[0]))  //address
    {
        sdt_int8u crc_value[2];
        Crc16CalculateOfByte(&mix_oper->receive_buff[0],(rd_length-2),&crc_value[0]);
        if((crc_value[1] == mix_oper->receive_buff[rd_length-2]) && (crc_value[0] == mix_oper->receive_buff[rd_length-1]))//crc
        {
            
            if(0x03 == mix_oper->receive_buff[1])
            {
                rx_reddReg_addr = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[2]);
                rx_reddReg_len = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[4]);
                if(rx_reddReg_len > max_reg_len)
                {//长度错误,应答
                    
                }
                else if(mix_pMdsRtu_oper->mdsRtu_parameter.cbk_read_modbus_slave_register(rx_reddReg_addr,rx_reddReg_len,&mix_pMdsRtu_oper->reg_details))
                {//读取完成,组帧应答
                    
                }
                else
                {//读取发生错误,应答
                    
                }

            }
            else if(0x06 == mix_oper->receive_buff[1])
            {
                mix_oper->writeReg_addr = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[2]);
                mix_oper->writeReg_length = 1;
                
                mix_oper->transmit_buff[0] = mix_oper->mRtu_parameter.mRtu_address;
                mix_oper->transmit_buff[1] = 0x06;
                mix_oper->transmit_buff[2] = mix_oper->receive_buff[2];
                mix_oper->transmit_buff[3] = mix_oper->receive_buff[3];
                mix_oper->transmit_buff[4] = mix_oper->receive_buff[4];
                mix_oper->transmit_buff[5] = mix_oper->receive_buff[5];
                mix_oper->transmit_length = 6;

                if(mix_pMdsRtu_oper->mdsRtu_parameter.cbk_write_modbus_slave_register(rx_writeReg_addr,1,&mix_pMdsRtu_oper->reg_details))
                {
                    
                }
                else
                {
                    
                }
            }
            else if(0x10 == mix_oper->receive_buff[1])
            {
                mix_oper->writeReg_addr = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[2]);
                mix_oper->writeReg_length = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[4]);
                
                mix_oper->transmit_buff[0] = mix_oper->mRtu_parameter.mRtu_address;
                mix_oper->transmit_buff[1] = 0x10;
                mix_oper->transmit_buff[2] = mix_oper->receive_buff[2];
                mix_oper->transmit_buff[3] = mix_oper->receive_buff[3];
                mix_oper->transmit_buff[4] = mix_oper->receive_buff[4];
                mix_oper->transmit_buff[5] = mix_oper->receive_buff[5];
                mix_oper->transmit_length = 6;

                if(mix_pMdsRtu_oper->mdsRtu_parameter.cbk_write_modbus_slave_register(rx_writeReg_addr,rx_writeReg_len,&mix_pMdsRtu_oper->reg_details))
                {
                    
                }
                else
                {
                    
                }
            }
            else if(0x17 == mix_oper->receive_buff[1])
            {
                rx_readReg_addr = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[2]);
                rx_readReg_len  = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[4]);
                rx_writeReg_addr = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[6]);
                rx_writeReg_len = pbc_arrayToInt16u_bigEndian(&mix_oper->receive_buff[8]);
                
                mix_oper->transmit_buff[0] = mix_oper->mRtu_parameter.mRtu_address;
                mix_oper->transmit_buff[1] = 0x17;
                mix_oper->transmit_buff[2] = mix_oper->readReg_length * 2;
                mix_oper->transmit_length = 3;

                if(mix_pMdsRtu_oper->mdsRtu_parameter.cbk_write_modbus_slave_register(rx_writeReg_addr,rx_writeReg_len,&mix_pMdsRtu_oper->reg_details))
                {
                    if(mix_pMdsRtu_oper->mdsRtu_parameter.cbk_read_modbus_slave_register(rx_readReg_addr,rx_readReg_len,&mix_pMdsRtu_oper->reg_details))
                    {//读取完成,组帧应答
                        
                    }
                    else
                    {//读取发生错误,应答
                    }                  
                }
                else
                {
                    
                }

            }
            else
            {
                return(sdt_false);
            }
            return(sdt_true);
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//重新开始字符宽度计时
//------------------------------------------------------------------------------
static void restart_bus_free_timer(mdsRtu_oper_def* mix_pMdsRtu_oper)
{
    mix_pMdsRtu_oper->timer_busfree_cnt = bsp_pull_us_tick();//获取us级的节拍数据
}
//------------------------------------------------------------------------------
//name:总线空闲字符宽带是否完成
//in:   in_chart_width 目前字符宽度，35代表3.5T的目标宽度
//out:  sdt_true 满足目标的字符空闲宽度
//------------------------------------------------------------------------------
static sdt_bool pull_bus_free_finish(mdsRtu_oper_def* mix_pMdsRtu_oper sdt_int8u in_chart_width)
{
    sdt_int16u rd_us_tick;
    sdt_int16u death_ticks;
    sdt_int32u expect_ticks;
    
    rd_us_tick = bsp_pull_us_tick();
    death_ticks = rd_us_tick - mix_pMdsRtu_oper->timer_busfree_cnt;

    expect_ticks = 1000000;
    expect_ticks = (expect_ticks*(sdt_int32u)in_chart_width)/mix_pMdsRtu_oper->mdsRtu_parameter.mdsRtu_baudrate;
    if(rd_us_tick > expect_ticks)
    {
        return(sdt_true);
    }
    return(sdt_false);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//modbus Rtu link状态跳变
//------------------------------------------------------------------------------
static void modbus_slave_status_jump(mdsRtu_oper_def* mix_pMdsRtu_oper)
{
    mdsRtu_run_status_def read_mdsRtu_run_status;
    
    do
    {
        read_mdsRtu_run_status = mix_pMdsRtu_oper->mdsRtu_run_status;
        switch(read_mdsRtu_run_status)
        {
            case mdsRtu_rs_idle:
            {
                mix_pMdsRtu_oper->entry_phy_rx();
                mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_rx_wait;
                break;
            }
            case mdsRtu_rs_rx_wait:
            {
                
                if(mdsRtu_bits_rx_byte & mix_pMdsRtu_oper->mdsRtu_bits)
                {
                    mix_pMdsRtu_oper->mdsRtu_bits &= ~mdsRtu_bits_rx_byte;
                    mix_pMdsRtu_oper->rx_buff[0] = mix_pMdsRtu_oper->rx_byte;
                    mix_pMdsRtu_oper->receive_len = 1;
                    mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_rx_doing;
                    
                    restart_bus_free_timer(mix_pMdsRtu_oper);
                    
                }
                break;
            }
            case mdsRtu_rs_rx_doing:
            {
                if(mdsRtu_bits_rx_byte & mix_pMdsRtu_oper->mdsRtu_bits)
                {
                    mix_pMdsRtu_oper->mdsRtu_bits &= ~mdsRtu_bits_rx_byte;
                    mix_pMdsRtu_oper->rx_buff[mix_pMdsRtu_oper->receive_len] = mix_pMdsRtu_oper->rx_byte;
                    mix_pMdsRtu_oper->receive_len ++;
                    
                    restart_bus_free_timer(mix_pMdsRtu_oper);
                }
                else
                {
                    if(pull_bus_free_finish(mix_pMdsRtu_oper,20))   //标准3.5T,检测2.0T视为报文完成
                    {
                        mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_rx_complete;
                    }  
                }
                break;
            }
            case mdsRtu_rs_rx_complete:
            {
                if(modbus_slave_protocol(mix_pMdsRtu_oper))
                {
                    mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_tx_start;  //开始应答
                }
                else
                {
                    mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_rx_wait;  //协议处理失败,重新开始接收
                }
                break;
            }
            case mdsRtu_rs_tx_start:
            {
                mix_pMdsRtu_oper->entry_phy_tx();
                mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_tx_35t
                restart_bus_free_timer(mix_pMdsRtu_oper);
                break;
            }
            case mdsRtu_rs_tx_35t:
            {
                if(pull_bus_free_finish(mix_pMdsRtu_oper,45))  //标准3.5T,发送多增加1T的时间，保证空闲时间充足
                {
                    mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_tx_doing;
                    mix_pMdsRtu_oper->transmit_index = 0;
                }
                break;
            }
            case mdsRtu_rs_tx_doing:
            {
                sdt_int16u surplus_bytes;
                
                if(mix_pMdsRtu_oper->transmit_index == mix_pMdsRtu_oper->transmit_len)  //数据转移完毕
                {
                    mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_tx_stop;
                }
                else
                {
                    surplus_bytes = mix_pMdsRtu_oper->transfet_bytes_tx_phy(&mix_pMdsRtu_oper->tx_buff[0],(mix_pMdsRtu_oper->transmit_len - mix_pMdsRtu_oper->transmit_index));
                    mix_pMdsRtu_oper->transmit_index = mix_pMdsRtu_oper->transmit_len - surplus_bytes;
                }
                break;
            }
            case mdsRtu_rs_tx_stop:
            {
                if(mix_pLog_oper->pull_complete_tx_data())
                {
                    restart_bus_free_timer(mix_pMdsRtu_oper);
                    mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_tx_complete;
                }
                break;
            }
            
            case mdsRtu_rs_tx_complete:
            {
                if(pull_bus_free_finish(mix_pMdsRtu_oper,25))//发送完毕，保持2.5T的总线时间,转入接收状态
                {
                    mix_pMdsRtu_oper->entry_phy_rx();
                    mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_rx_wait;
                }
                break;
            }
            default:
            {
                mix_pMdsRtu_oper->mdsRtu_run_status = mdsRtu_rs_idle;
                break;
            }
        }
    }while(read_mdsRtu_run_status != mix_pMdsRtu_oper->mdsRtu_run_status);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//独立模块任务
//------------------------------------------------------------------------------
static void alone_modbus_slave_task(mdsRtu_oper_def* mix_pMdsRtu_oper)
{
    mix_pMdsRtu_oper->pull_look_for_byte_rx();
    modbus_slave_status_jump(mix_pMdsRtu_oper);
}
//++++++++++++++++++++++++++++++++++++interface+++++++++++++++++++++++++++++++++
static sdt_bool mdsRtu_cfged = sdt_false;
//------------------------------------------------------------------------------
void mde_modbus_slave_task(void)
{
    if(mdsRtu_cfged)
    {
        sdt_int8u i;
        for(i = 0;i < max_solid;i ++)
        {
            alone_modbus_slave_task(&mdsRtu_oper_solid[i]);
        }
    }
    else
    {
        mdsRtu_cfged = sdt_true;
        mdsRtu_solid_cfg();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:推送参数设置信息到模块
//fun:address,baudrate,callback
//in:   in_solid_number -- 实列号
//       参数指针
//out:  none
//------------------------------------------------------------------------------
void mde_push_modbus_slave_parameter(sdt_int8u in_solid_number,mdsRtu_parameter_def* mix_pMdsRtu_parameter)
{
    sdt_bool phy_reload_once = sdt_false;
    
    if(sdt_false == mdsRtu_cfged)
    {
        mdsRtu_cfged = sdt_true;
        mdsRtu_solid_cfg();
    }
    if(in_solid_number < max_solid)
    {
        mdsRtu_oper_solid[in_solid_number].mdsRtu_parameter.mdsRtu_address = mix_pMdsRtu_parameter->mdsRtu_address;
        
        if(mdsRtu_oper_solid[in_solid_number].mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_baudrate != mix_pMdsRtu_parameter->mdsRtu_phyProperty.mdsRtu_baudrate)
        {
            phy_reload_once = sdt_true;
        }
        if(mdsRtu_oper_solid[in_solid_number].mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_sysFrequency != mix_pMdsRtu_parameter->mdsRtu_phyProperty.mdsRtu_sysFrequency)
        {
            phy_reload_once = sdt_true;
        }
        if(mdsRtu_oper_solid[in_solid_number].mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_parity != mix_pMdsRtu_parameter->mdsRtu_phyProperty.mdsRtu_parity)
        {
            phy_reload_once = sdt_true;
        }
        if(mdsRtu_oper_solid[in_solid_number].mdsRtu_parameter.mdsRtu_phyProperty.mdsRtu_stopBits != mix_pMdsRtu_parameter->mdsRtu_phyProperty.mdsRtu_stopBits)
        {
            phy_reload_once = sdt_true;
        }
        mdsRtu_oper_solid[in_solid_number].mdsRtu_parameter.cbk_read_modbus_slave_register = mix_pMdsRtu_parameter->cbk_read_modbus_slave_register;
        mdsRtu_oper_solid[in_solid_number].mdsRtu_parameter.cbk_write_modbus_slave_register = mix_pMdsRtu_parameter->cbk_write_modbus_slave_register;
        
        if(phy_reload_once)//重载PHY
        {
            mdsRtu_oper_solid[in_solid_number].phy_reload();
        }
    }
    else
    {
        while(1);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:推入一字节的接收数据到链路,一般由bsp层调用
//in:   in_solid_number -- 实列号
//      in_rx_byte      -- 接收的数据
//out:  none
//------------------------------------------------------------------------------
void mde_push_modbus_slave_receive_byte(sdt_int8u in_solid_number,sdt_int8u in_rx_byte)
{
    if(in_solid_number < max_solid)
    {
        mdsRtu_oper_solid[in_solid_number].mdsRtu_bits |= mdsRtu_bits_rx_byte;
        mdsRtu_oper_solid[in_solid_number].rx_byte = in_rx_byte;
        modbus_slave_status_jump(&mdsRtu_oper_solid[in_solid_number]);//数据进入后,马上处理状态
    }
    else
    {
        while(1);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++