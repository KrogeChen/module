//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef snail_data_types
    #include ".\snail_data_types.h"
#endif
//-------------------------------------------------------------------------------------------------
#include "..\mde_modbus_slave.h"
#include "n32g45x.h"
#include "intrinsics.h"
//-------------------------------------------------------------------------------------------------
//UART5
//TXD PC12,RXD PD2,SELECT PB3
//-------------------------------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//input floating
#define macro_trs_cfg    //PG_DDR |= (0x04); PG_CR1 &= (~0x04); PG_CR2 &= (~0x04); PG_ODR &= (~0x04)
#define macro_trs_receive       GPIO_ResetBits(GPIOB,GPIO_PIN_3);;
#define macro_trs_transmit      GPIO_SetBits(GPIOB,GPIO_PIN_3);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-------------------------------------------------------------------------------------------------
void bsp_phy0_cfg(void)
{
    GPIO_InitType    GPIO_USART5INIT; 
    USART_InitType   USART5_INIT;
    NVIC_InitType    USART5_NVIC_INIT;

 //-----------------------------------------------------------------------------   
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART5,ENABLE);  //开启串口时钟
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD,ENABLE);   //开启GPIOD口时钟
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC,ENABLE);   //开启GPIOC口时钟
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB,ENABLE);   //开启GPIOB口时钟
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA,ENABLE);   //开启GPIOA口时钟
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);

    GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_SW_ENABLE, ENABLE);

//-----------------------------------------------------------------------------  
    GPIO_USART5INIT.Pin=GPIO_PIN_12;                   //PC12 U5_TXD
    GPIO_USART5INIT.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_USART5INIT.GPIO_Mode=GPIO_Mode_AF_PP;            //输出复用推挽
    GPIO_InitPeripheral(GPIOC,&GPIO_USART5INIT);
//-----------------------------------------------------------------------------     
    GPIO_USART5INIT.Pin=GPIO_PIN_2;                  //PD2 U5_RXD
    GPIO_USART5INIT.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_USART5INIT.GPIO_Mode=GPIO_Mode_IPU;               //输入上拉
    GPIO_InitPeripheral(GPIOD,&GPIO_USART5INIT);
//-----------------------------------------------------------------------------  
    GPIO_USART5INIT.Pin=GPIO_PIN_3;                   //PB3 TRSEL
    GPIO_USART5INIT.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_USART5INIT.GPIO_Mode=GPIO_Mode_Out_PP;            //推挽输出
    GPIO_InitPeripheral(GPIOB,&GPIO_USART5INIT);
    GPIO_ResetBits(GPIOB,GPIO_PIN_3);
//-----------------------------------------------------------------------------   
    USART5_INIT.BaudRate=9600;
    USART5_INIT.Parity=USART_PE_NO;
    USART5_INIT.WordLength=USART_WL_8B;
    USART5_INIT.StopBits=USART_STPB_1;
    USART5_INIT.HardwareFlowControl=USART_HFCTRL_NONE;
    USART5_INIT.Mode=(USART_MODE_RX+USART_MODE_TX);  //收发使能
//-----------------------------------------------------------------------------
    USART_Init(UART5,&USART5_INIT);
    USART_ConfigInt(UART5,USART_INT_RXDNE,ENABLE);           //使能接收中断
    USART_Enable(UART5,ENABLE);
//-----------------------------------------------------------------------------
    USART5_NVIC_INIT.NVIC_IRQChannel=UART5_IRQn;
    USART5_NVIC_INIT.NVIC_IRQChannelPreemptionPriority=0;  //主优先级
    USART5_NVIC_INIT.NVIC_IRQChannelSubPriority=0;         //子优先级
    USART5_NVIC_INIT.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&USART5_NVIC_INIT);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    TIM_TimeBaseInitType  TIM_TimeBaseStructure;              //TIM4 3.5T的时序管理
    NVIC_InitType NVIC_InitStructure;                         //
//----------------------------------------------------------------------------- 
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM4, ENABLE); 
//-----------------------------------------------------------------------------
    TIM_DeInit(TIM4);
    /* Time base configuration */    
    TIM_TimeBaseStructure.Prescaler =72-1;                    //72M 1000K
    TIM_TimeBaseStructure.Period = 0xffff  ;                      // 当定时器从0计数到999，即为1000次，为一个定时周期,1个unit 10us
    TIM_TimeBaseStructure.ClkDiv = 0 ;        //设置时钟分频系数：不分频(这里用不到)
    TIM_TimeBaseStructure.CntMode = TIM_CNT_MODE_UP;     //向上计数模式
    TIM_InitTimeBase(TIM4, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4,TIM_FLAG_UPDATE);
    TIM_Enable(TIM4,ENABLE); 
    //while(SET!=TIM_GetFlagStatus(TIM4,TIM_FLAG_UPDATE));
    //TIM_Enable(TIM4,DISABLE); 
//-----------------------------------------------------------------------------
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;       
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
//-----------------------------------------------------------------------------
    //TIM_ClrIntPendingBit(TIM4, TIM_INT_UPDATE);
    //TIM_ClearFlag(TIM4,TIM_FLAG_UPDATE);               //清除溢出中断标志  
    //TIM_ConfigInt(TIM4,TIM_INT_UPDATE,ENABLE);  
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
volatile static sdt_int8u tx_buffer[256];
static sdt_int8u tx_idx_str,tx_idx_end;
//------------------------------------------------------------------------------
volatile static sdt_int8u rx_buffer[256];
static sdt_int8u rx_idx_str,rx_idx_end;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_bool tx_complete;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//中断服务函数
//-----------------------------------------------------------------------------
void UART5_IRQHandler(void)
{
    static sdt_int8u Read_reg;
    if((SET==USART_GetFlagStatus(UART5,USART_FLAG_OREF))||\
       (SET==USART_GetFlagStatus(UART5,USART_FLAG_NEF))||\
       (SET==USART_GetFlagStatus(UART5,USART_FLAG_FEF)))
    {
        USART_GetFlagStatus(UART5,USART_FLAG_OREF);
        USART_ReceiveData(UART5);
    }
    else if(SET==USART_GetFlagStatus(UART5,USART_FLAG_PEF))
    {
    }
    else if(SET==USART_GetFlagStatus(UART5,USART_FLAG_RXDNE))
    {
        Read_reg=USART_ReceiveData(UART5);
        rx_buffer[rx_idx_end] = Read_reg;
        rx_idx_end ++;
    }
    if(SET==USART_GetIntStatus(UART5,USART_INT_TXDE))
    {
        if(SET==USART_GetFlagStatus(UART5,USART_FLAG_TXDE))
        {
            if(tx_idx_str != tx_idx_end)
            {
                Read_reg = tx_buffer[tx_idx_str];
                tx_idx_str ++;
                USART_SendData(UART5,Read_reg);
            }
            else
            {
                USART_ConfigInt(UART5,USART_INT_TXDE,DISABLE);
            }
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void bsp_look_for_byte_rx_phy0(void)
{
    sdt_int8u now_rx_byte;
    sdt_bool buffer_empty;
    
    do
    {
        buffer_empty = sdt_true;
        __disable_interrupt(); 
        if(rx_idx_str != rx_idx_end)
        {
            now_rx_byte = rx_buffer[rx_idx_str];
            rx_idx_str ++;
            buffer_empty = sdt_false;
        }
        __enable_interrupt();
        if(sdt_false == buffer_empty)
        {
            mde_push_modbus_slave_receive_byte(0,now_rx_byte);
        }
    }while(sdt_false == buffer_empty);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_bool start_cpt;
//------------------------------------------------------------------------------
sdt_int16u bsp_transfet_bytes_tx_phy0(sdt_int8u* in_pByte,sdt_int16u in_expect_bytes)
{

    while(0 != in_expect_bytes)
    {
        __disable_interrupt(); 
        tx_buffer[tx_idx_end] = *in_pByte;
        tx_idx_end ++;
        __enable_interrupt();
        in_pByte ++;
        in_expect_bytes --;
    }
    USART_ConfigInt(UART5,USART_INT_TXDE,ENABLE);
    return(in_expect_bytes);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_bool bsp_pull_complete_tx_phy0(void)
{
    if(USART_GetFlagStatus(UART5,USART_FLAG_TXC))
    {
        return(sdt_true);
    }
    return(sdt_false);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void bsp_entry_phy_rx_phy0(void)
{
    macro_trs_receive;
    UART5->CTRL1|=0x00000004;  //RE  Receiver is Enable
    USART_ConfigInt(UART5,USART_INT_RXDNE,ENABLE);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//对等总线，由发送报文时选择发送方式，此处采用空函数
//------------------------------------------------------------------------------
void bsp_entry_phy_tx_phy0(void)
{
    UART5->CTRL1&=~0x00000004;  //RE  Receiver is Disable
    USART_ConfigInt(UART5,USART_INT_RXDNE,ENABLE);
    macro_trs_transmit
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void bsp_phy_reload_phy0(void)
{
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_int16u bsp_pull_us_tick(void)
{
    return(TIM4->CNT);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
