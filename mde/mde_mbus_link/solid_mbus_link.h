//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//mbus link solid
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef solid_mbus_link_half_H
#define solid_mbus_link_half_H
//------------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++solid+++++++++++++++++++++++++++++++++++++++++
#define max_solid    1
//-----------------------------------------------------------------------------
static mbslk_oper_def mbslk_oper_solid[max_solid];
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void mbslk_solid_cfg(void)
{
//-----------------------------------------------------------------------------
    bsp_phy0_cfg();
    
    mbslk_oper_solid[0].pull_look_for_byte_rx = bsp_look_for_byte_rx_phy0;
    mbslk_oper_solid[0].transfet_bytes_to_phy_tx = bsp_transfet_bytes_tx_phy0;
    mbslk_oper_solid[0].pull_complete_tx_data = bsp_pull_complete_tx_phy0;
    mbslk_oper_solid[0].phy_baudrate_calibrate = bsp_phy_baudrate_calibrate_phy0;
    mbslk_oper_solid[0].entry_phy_rx = bsp_entry_phy_rx_phy0;
    mbslk_oper_solid[0].entry_phy_tx = bsp_entry_phy_tx_phy0;


    mbslk_oper_solid[0].timer_brate_cal.timStatusBits &= ~timStatusBits_typeMask;
    mbslk_oper_solid[0].timer_brate_cal.timStatusBits |= timStatusBits_typeSecond;
 
//-----------------------------------------------------------------------------
}
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
