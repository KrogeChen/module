//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef snail_data_types
    #include ".\snail_data_types.h"
#endif
//-------------------------------------------------------------------------------------------------
#include "..\mde_mbus_link.h"
#include "hc32l13x.h"
#include "intrinsics.h"
#include "interrupts_hc32l13x.h"
//-------------------------------------------------------------------------------------------------
  /**
  *******************************************************************************
  ** \brief DMA传输当前状态
  **
  ******************************************************************************/
  typedef enum en_dma_stat
  {
    DmaDefault                                = 0U,    ///< 初始值
    DmaAddOverflow                                    = 1U,    ///< 传输错误引起中止（地址溢出）
    DmaHALT                         = 2U,    ///< 传输停止请求引起中止（外设停止请求引起的停止或者EB/DE位引起的禁止传输）
    DmaAccSCRErr                    = 3U,    ///< 传输错误引起中止（传输源地址访问错误）
    DmaAccDestErr                   = 4U,    ///< 传输错误引起中止（传输目的地址访问错误）
    DmaTransferComplete             = 5U,    ///< 成功传输完成
    DmaTransferPause                = 7U,    ///< 传输暂停      
  } en_dma_stat_t;
  /**
  *******************************************************************************
  ** \brief DMA 触发源选择
  **
  ******************************************************************************/  
  typedef enum stc_dma_trig_sel
  {
    DmaSWTrig                         = 0U,     ///< Select DMA software trig 
    DmaSPI0RXTrig                     = 32U,    ///< Select DMA hardware trig 0
    DmaSPI0TXTrig                     = 33U,    ///< Select DMA hardware trig 1
    DmaSPI1RXTrig                     = 34U,    ///< Select DMA hardware trig 2
    DmaSPI1TXTrig                     = 35U,    ///< Select DMA hardware trig 3
    DmaADCJQRTrig                     = 36U,    ///< Select DMA hardware trig 4
    DmaADCSQRTrig                     = 37U,    ///< Select DMA hardware trig 5
    DmaLCDTxTrig                      = 38U,    ///< Select DMA hardware trig 6
    DmaUart0RxTrig                    = 40U,    ///< Select DMA hardware trig 8
    DmaUart0TxTrig                    = 41U,    ///< Select DMA hardware trig 9             
    DmaUart1RxTrig                    = 42U,    ///< Select DMA hardware trig 10
    DmaUart1TxTrig                    = 43U,    ///< Select DMA hardware trig 11
    DmaLpUart0RxTrig                  = 44U,    ///< Select DMA hardware trig 12
    DmaLpUart0TxTrig                  = 45U,    ///< Select DMA hardware trig 13
    DmaLpUart1RxTrig                  = 46U,    ///< Select DMA hardware trig 14
    DmaLpUart1TxTrig                  = 47U,    ///< Select DMA hardware trig 15
    DmaTIM0ATrig                      = 50U,    ///< Select DMA hardware trig 18
    DmaTIM0BTrig                      = 51U,    ///< Select DMA hardware trig 19
    DmaTIM1ATrig                      = 52U,    ///< Select DMA hardware trig 20
    DmaTIM1BTrig                      = 53U,    ///< Select DMA hardware trig 21
    DmaTIM2ATrig                      = 54U,    ///< Select DMA hardware trig 22   
    DmaTIM2BTrig                      = 55U,    ///< Select DMA hardware trig 23   
    DmaTIM3ATrig                      = 56U,    ///< Select DMA hardware trig 24    
    DmaTIM3BTrig                      = 57U,    ///< Select DMA hardware trig 25   
    DmaTIM4ATrig                      = 58U,    ///< Select DMA hardware trig 26    
    DmaTIM4BTrig                      = 59U,    ///< Select DMA hardware trig 27   
    DmaTIM5ATrig                      = 60U,    ///< Select DMA hardware trig 28    
    DmaTIM5BTrig                      = 61U,    ///< Select DMA hardware trig 29   
    DmaTIM6ATrig                      = 62U,    ///< Select DMA hardware trig 30    
    DmaTIM6BTrig                      = 63U,    ///< Select DMA hardware trig 31   
  }en_dma_trig_sel_t;  
//-------------------------------------------------------------------------------------------------
//uart0 DMA 传输
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_int8u dma_receive_buff[128];
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//PHY use uart0 PA3--U2RX  PA2--U2TX
//tr_select  PB1
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//input floating
#define macro_trs_cfg    //PG_DDR |= (0x04); PG_CR1 &= (~0x04); PG_CR2 &= (~0x04); PG_ODR &= (~0x04)
#define macro_trs_receive       M0P_GPIO->PADIR_f.PA08 = 1;
#define macro_trs_transmit      M0P_GPIO->PADIR_f.PA08 = 0;M0P_GPIO->PAOUT_f.PA08 = 1;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define bgk_s0    0
static sdt_int32u rx_buff_addr_backup;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-------------------------------------------------------------------------------------------------
void bsp_phy0_cfg(void)
{
    M0P_SYSCTRL->PERI_CLKEN_f.DMA = 1;
    M0P_SYSCTRL->PERI_CLKEN_f.UART0 = 1;
    M0P_SYSCTRL->PERI_CLKEN_f.BASETIM = 1;
    M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1;
//------------------------------------------------------------------------------
    //PB08 uart0_txd,PB09 uart0_rxd,TXEN  PA08,BY  PB07
    M0P_GPIO->PB08_SEL = 7;
    M0P_GPIO->PB09_SEL = 7;
    M0P_GPIO->PBDIR_f.PB08 = 0;  //output
    M0P_GPIO->PBOUT_f.PB08 = 1;
    M0P_GPIO->PBDIR_f.PB09 = 1;  //input
    
    M0P_GPIO->PA08_SEL = 0;      //input  TXEN
    M0P_GPIO->PAPD_f.PA08 = 0;
    M0P_GPIO->PAPU_f.PA08 = 0;
    M0P_GPIO->PAOD_f.PA08 = 0;   //推挽输出
    M0P_GPIO->PAOUT_f.PA08 = 1;
    M0P_GPIO->PADIR_f.PA08 = 1;  //input

//    M0P_GPIO->PB07_SEL = 0;      //input  BY busy,下降延中断
//    M0P_GPIO->PBPD_f.PB07 = 0;
//    M0P_GPIO->PBPU_f.PB07 = 0;
//    M0P_GPIO->PBFIE_f.PB07 = 1;
//    EnableNvic(PORTB_IRQn,IrqLevel3,TRUE);
//------------------------------------------------------------------------------timer0,超时安全保护
    M0P_TIM0_MODE0->M0CR_f.MODE = 0;
    M0P_TIM0_MODE0->M0CR_f.UIE = 1;
    M0P_TIM0_MODE0->M0CR_f.MD = 1;
    M0P_TIM0_MODE0->M0CR_f.CT = 0;
    M0P_TIM0_MODE0->M0CR_f.PRS = 3; // 24/8 = 4M
    M0P_TIM0_MODE0->ARR = (0xffff - 8000);  //2ms
    M0P_TIM0_MODE0->CNT = (0xffff - 8000);
    M0P_TIM0_MODE0->M0CR_f.CTEN = 0;
    EnableNvic(TIM0_IRQn,IrqLevel3,TRUE);
//------------------------------------------------------------------------------
    M0P_DMAC->CONF = 0x00000000;
    M0P_DMAC->CONFA0_f.ENS = 1;
    M0P_DMAC->CONFA0_f.TRI_SEL = DmaUart0RxTrig;//DMA0 uart0_rx
    M0P_DMAC->CONFA0_f.BC = 0;
    M0P_DMAC->CONFA0_f.TC = sizeof(dma_receive_buff) - 1;
    
    M0P_DMAC->CONFB0_f.MODE = 00u; //block
    M0P_DMAC->CONFB0_f.WIDTH = 00u; //8bits
    M0P_DMAC->CONFB0_f.FS = 1; //源地址不变
    M0P_DMAC->CONFB0_f.FD = 0; //目标地址自增
    M0P_DMAC->CONFB0_f.RC = 1; //TC重载
    M0P_DMAC->CONFB0_f.RS = 1; //地址重载
    M0P_DMAC->CONFB0_f.RD = 1;
    M0P_DMAC->CONFB0_f.ERR_IE = 1;
    M0P_DMAC->CONFB0_f.FIS_IE = 1;
    M0P_DMAC->CONFB0_f.STAT = 0;
    M0P_DMAC->CONFB0_f.MSK = 0;
    M0P_DMAC->SRCADR0 = (sdt_int32u)&M0P_UART0->SBUF;
    M0P_DMAC->DSTADR0 = (sdt_int32u)&dma_receive_buff[0];
    rx_buff_addr_backup = (sdt_int32u)&dma_receive_buff[0];
//------------------------------------------------------------------------------
    M0P_DMAC->CONF = 0x00000000;
    //M0P_DMAC->CONFA1_f.ENS = 1;
    M0P_DMAC->CONFA1_f.TRI_SEL = DmaUart0TxTrig;//DMA0 uart0_tx
    M0P_DMAC->CONFA1_f.BC = 0;
    M0P_DMAC->CONFA1_f.TC = 0;

    M0P_DMAC->CONFB1_f.MODE = 00u;  //block
    M0P_DMAC->CONFB1_f.WIDTH = 00u; //8bits
    M0P_DMAC->CONFB1_f.FS = 0; //源地址自增
    M0P_DMAC->CONFB1_f.FD = 1; //目标地址不变
    M0P_DMAC->CONFB1_f.RC = 0; //TC不重载
    M0P_DMAC->CONFB1_f.RS = 0; //地址重载
    M0P_DMAC->CONFB1_f.RD = 1;
    M0P_DMAC->CONFB1_f.ERR_IE = 1;
    M0P_DMAC->CONFB1_f.FIS_IE = 1;
    M0P_DMAC->CONFB1_f.STAT = 0;
    M0P_DMAC->CONFB1_f.MSK = 0;
    M0P_DMAC->SRCADR1 = (sdt_int32u)&dma_receive_buff[0];
    M0P_DMAC->DSTADR1 = (sdt_int32u)&M0P_UART0->SBUF;
    
    M0P_DMAC->CONF_f.EN = 1;
//------------------------------------------------------------------------------
    M0P_UART0->SCON_f.SM = 01;
    M0P_UART0->SCON_f.DMARXEN = 1;
    M0P_UART0->SCON_f.DMATXEN = 1;
    M0P_UART0->SCON_f.REN = 1;
    M0P_UART0->SCON_f.FEIE = 1; //帧错误中断
    M0P_UART0->SCNT = 156;  //24M -- 9600
    EnableNvic(UART0_IRQn,IrqLevel3,TRUE);
//------------------------------------------------------------------------------
    EnableNvic(DMAC_IRQn,IrqLevel3,TRUE);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_bool line_busy = sdt_false;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void PORTB_IRQHandler(void)
{
//    if(1 == M0P_GPIO->PB_STAT_f.PB07)
//    {
//        M0P_GPIO->PB_ICLR_f.PB07 = 0;
////        if(1 == M0P_UART0->ISR_f.RC)
////        {
////            M0P_UART0->ICR_f.RCCF = 0;
////        }
//        line_busy = sdt_true;
////        M0P_TIM0_MODE0->M0CR_f.CTEN = 0;
////        M0P_TIM0_MODE0->CNT = (0xffff - 48000);
////        M0P_TIM0_MODE0->M0CR_f.CTEN = 1;
//    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_bool tx_complete;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TIM0_IRQHandler(void)
{
    if(1 == M0P_TIM0_MODE0->IFR_f.UIF)
    {
        M0P_TIM0_MODE0->M0CR_f.CTEN = 0;
        M0P_TIM0_MODE0->ICLR_f.UIF = 0;
        tx_complete = sdt_true;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void UART0_IRQHandler(void)
{
    if(1 == M0P_UART0->ISR_f.FE)
    {
        M0P_UART0->ICR_f.FECF = 0;
        line_busy = sdt_false;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DMAC_IRQHandler(void)
{
    if(DmaTransferComplete == M0P_DMAC->CONFB0_f.STAT)
    {
        M0P_DMAC->CONFB0_f.STAT = 0;
        M0P_DMAC->CONFA0_f.ENS = 1;        
    }
    else if((DmaAddOverflow == M0P_DMAC->CONFB0_f.STAT) ||\
            (DmaHALT == M0P_DMAC->CONFB0_f.STAT) ||\
            (DmaAccSCRErr == M0P_DMAC->CONFB0_f.STAT) ||\
            (DmaAccDestErr == M0P_DMAC->CONFB0_f.STAT))//错误，
    {
        M0P_DMAC->CONFB0_f.STAT = 0;
        M0P_DMAC->CONFA0_f.ENS = 1;  
    }

    if(DmaTransferComplete == M0P_DMAC->CONFB1_f.STAT)
    {
        M0P_DMAC->CONFB1_f.STAT = 0;
    }
    else if((DmaAddOverflow == M0P_DMAC->CONFB1_f.STAT) ||\
            (DmaHALT == M0P_DMAC->CONFB1_f.STAT) ||\
            (DmaAccSCRErr == M0P_DMAC->CONFB1_f.STAT) ||\
            (DmaAccDestErr == M0P_DMAC->CONFB1_f.STAT))//错误，重载DMA参数
    {
        M0P_DMAC->CONFB1_f.STAT = 0;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void bsp_look_for_byte_rx_phy0(void)
{
    sdt_int8u now_rx_byte;
    
    while(rx_buff_addr_backup != M0P_DMAC->DSTADR0)  //一次性读取缓冲区数据
    {
        now_rx_byte = *(sdt_int8u*)rx_buff_addr_backup;
        push_mbus_one_receive_byte(bgk_s0,now_rx_byte);     //压入一个字节的数据到link
        rx_buff_addr_backup ++;
        if(rx_buff_addr_backup > (sdt_int32u)&dma_receive_buff[sizeof(dma_receive_buff) - 1])
        {
            rx_buff_addr_backup = (sdt_int32u)&dma_receive_buff[0];
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static sdt_bool start_cpt;
//------------------------------------------------------------------------------
sdt_int16u bsp_transfet_bytes_tx_phy0(sdt_int8u* in_pByte,sdt_int16u in_expect_bytes)
{

    if(0 != in_expect_bytes)
    {
        M0P_DMAC->SRCADR1 = (sdt_int32u)in_pByte; //DMA一次发送完毕
        M0P_DMAC->CONFA1_f.TC = in_expect_bytes - 1;
        M0P_DMAC->CONFA1_f.ENS = 1;       
        M0P_UART0->ICR_f.TCCF = 0;
        
        start_cpt = sdt_false;
    }
    return(0);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_bool bsp_pull_complete_tx_phy0(void)
{
    if(0 != M0P_UART0->ISR_f.TXE) //结束标志,采用定时器延时3个字符
    {
        if(start_cpt)
        {
            if(tx_complete)
            {
                return(sdt_true);
            }            
        }
        else
        {
            start_cpt = sdt_true;
            tx_complete = sdt_false;
            M0P_TIM0_MODE0->M0CR_f.CTEN = 0;
            M0P_TIM0_MODE0->CNT = (0xffff - 12000);  //3ms
            M0P_TIM0_MODE0->M0CR_f.CTEN = 1;
        }
    }
    else
    {
        start_cpt = sdt_false;
    }

    return(sdt_false);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_bool bsp_pull_pyh_busy_phy0(void)
{
    if(1 == M0P_UART0->ISR_f.RC)
    {
        M0P_UART0->ICR_f.RCCF = 0;
        line_busy = sdt_false;
    }
    if(line_busy)
    {
        return(sdt_true);
    }
    return(sdt_false);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void bsp_phy_baudrate_calibrate_phy0(void)
{
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void bsp_entry_phy_rx_phy0(void)
{
    macro_trs_receive;
    M0P_UART0->ICR_f.RCCF = 0;
    M0P_UART0->SCON_f.REN = 1;  //开启接收器
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//对等总线，由发送报文时选择发送方式，此处采用空函数
//------------------------------------------------------------------------------
void bsp_entry_phy_tx_phy0(void)
{
    M0P_UART0->SCON_f.REN = 0;  //关闭接收器
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
