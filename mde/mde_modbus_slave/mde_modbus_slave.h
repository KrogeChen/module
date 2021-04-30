#ifndef mde_modbus_slave_H
#define mde_modbus_slave_H
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef snail_data_types
    #include ".\depend\snail_data_types.h"
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++     
//------------------------------------------------------------------------------
//modbus RTU slave 半双工模块
//------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef enum
{
    mdsRtu_parity_none  = 0x00,
    mdsRtu_parity_even  = 0x01,
    mdsRtu_parity_odd   = 0x02,
}mdsRtu_parity_def;
//-----------------------------------------------------------------------------
typedef enum
{
    mdsRtu_stopBits_one = 0x00,
    mdsRtu_stopBits_two = 0x01,
}mdsRtu_stopBits_def;
//-----------------------------------------------------------------------------
typedef struct
{
    sdt_int32u            mdsRtu_baudrate;
    sdt_int32u            mdsRtu_sysFrequency;
    mdsRtu_parity_def     mdsRtu_parity;
    mdsRtu_stopBits_def   mdsRtu_stopBits;    
}mdsRtu_phyProperty_def;
//-----------------------------------------------------------------------------
typedef struct 
{
    sdt_int8u               mdsRtu_address;
    mdsRtu_phyProperty_def  mdsRtu_phyProperty;

    //callback function
    sdt_bool(*cbk_read_modbus_slave_register)(sdt_int16u in_reg_addr,sdt_int8u in_reg_length,sdt_int16u* out_pWords);
    sdt_bool(*cbk_write_modbus_slave_register)(sdt_int16u in_reg_addr,sdt_int8u in_reg_length,sdt_int16u* in_pWords);
} mdsRtu_parameter_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++task++++++++++++++++++++++++++++++++++++++++
//名称: modbus slave模块任务
//功能: 负责modbus数据的收发和处理
//入口: 无
//
//出口: 无
//------------------------------------------------------------------------------
void mde_modbus_slave_task(void);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++interface+++++++++++++++++++++++++++++++++++++
//name:推送参数设置信息到模块
//fun:address,baudrate,callback
//in:   in_solid_number -- 实列号
//       参数指针
//out:  none
//------------------------------------------------------------------------------
void mde_push_modbus_slave_parameter(sdt_int8u in_solid_number,mdsRtu_parameter_def* mix_pMdsRtu_parameter);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:推入一字节的接收数据到链路,一般由bsp层调用
//in:   in_solid_number    -- 实列号
//      in_rx_byte      -- 接收的数据
//out:  none
//------------------------------------------------------------------------------
void mde_push_modbus_slave_receive_byte(sdt_int8u in_solid_number,sdt_int8u in_rx_byte);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++