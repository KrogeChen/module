#ifndef mde_modbus_master_H
#define mde_modbus_master_H
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef snail_data_types
    #include ".\depend\snail_data_types.h"
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++     
//------------------------------------------------------------------------------
//modbus RTU master 半双工模块
//支持 03 04 06 16 23 命令
//------------------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef enum
{
    mdsRtu_parity_reserve = 0x00, //保留
    mdsRtu_parity_none    = 0x01, //无校验
    mdsRtu_parity_even    = 0x02, //偶校验
    mdsRtu_parity_odd     = 0x03, //奇校验
}mdmRtu_parity_def;
//-----------------------------------------------------------------------------
typedef enum
{
    mdsRtu_stopBits_reserve = 0x00, //保留
    mdsRtu_stopBits_one     = 0x01, //1位停止位
    mdsRtu_stopBits_two     = 0x02, //2位停止位
}mdmRtu_stopBits_def;
//-----------------------------------------------------------------------------
typedef struct
{
    sdt_int32u            mdsRtu_baudrate;     //波特率
    sdt_int32u            mdsRtu_sysFrequency; //系统频率
    mdsRtu_parity_def     mdsRtu_parity;       //校验方式
    mdsRtu_stopBits_def   mdsRtu_stopBits;     //停止位数
}mdmRtu_phyProperty_def;
//-----------------------------------------------------------------------------
typedef struct 
{
    mdsRtu_phyProperty_def  mdsRtu_phyProperty;

    //callback function
    sdt_bool(*cbk_modbus_master_writeCmd_pull_register)(sdt_int8u in_slave_addr,sdt_int16u in_reg_addr,sdt_int8u in_reg_length,sdt_int16u* out_pWords);
    sdt_bool(*cbk_modbus_master_writeCmd_answer)(sdt_int8u in_slave_addr,sdt_int16u in_reg_addr,sdt_int8u in_reg_length);
    sdt_bool(*cbk_modbus_master_readCmd_answer)(sdt_int8u in_slave_addr,sdt_int16u in_reg_addr,sdt_int8u in_reg_length,sdt_int16u* in_pWords);
}mdmRtu_parameter_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef struct
{
    sdt_int8u   modbus_cmd;//命令
    sdt_int8u   slave_addr;//从机地址
    sdt_int16u  read_reg_address;//寄存器地址,读命令
    sdt_int16u  read_reg_length;//寄存器长度
    sdt_int16u  write_reg_address;//寄存器地址,写命令
    sdt_int16u  write_reg_length;//寄存器长度
}mdmRtu_slave_query_def;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++task++++++++++++++++++++++++++++++++++++++++
//名称: modbus master模块任务
//功能: 负责modbus数据的收发和处理
//入口: 无
//
//出口: 无
//------------------------------------------------------------------------------
void mde_modbus_master_task(void);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++interface+++++++++++++++++++++++++++++++++++++
//name:推送参数设置信息到模块
//fun:address,baudrate,callback
//in:   in_solid_number          实列号
//      mix_pMdsRtu_parameter    参数指针
//out:  none
//------------------------------------------------------------------------------
void mde_push_modbus_master_parameter(sdt_int8u in_solid_number,mdsRtu_parameter_def* mix_pMdsRtu_parameter);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name: 发送一次轮询命令
//fun:  地址,命令号,寄存器地址,寄存器长度
//in:   in_solid_number          实列号
//      in_pSlave_query          从机轮询数据
//out:  sdt_false 调用失败,命令不支持,长度超出范围
//------------------------------------------------------------------------------
sdt_bool mde_modbus_master_query_once(sdt_int8u in_solid_number,mdmRtu_slave_query_def* in_pSlave_query);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//name:推入一字节的接收数据到链路,一般由bsp层调用
//in:   in_solid_number    实列号
//      in_rx_byte         接收的数据
//out:  none
//------------------------------------------------------------------------------
void mde_push_modbus_slave_receive_byte(sdt_int8u in_solid_number,sdt_int8u in_rx_byte);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++