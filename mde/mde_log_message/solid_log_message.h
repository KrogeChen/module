//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//mbus link solid
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef solid_log_message_H
#define solid_log_message_H
//------------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++solid+++++++++++++++++++++++++++++++++++++++++
#define max_solid    1
//-----------------------------------------------------------------------------
static log_oper_def log_oper_solid[max_solid];
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void log_solid_cfg(void)
{
//-----------------------------------------------------------------------------
    bsp_phy0_cfg();
    
    log_oper_solid[0].pull_look_for_byte_rx = bsp_look_for_byte_rx_phy0;
    log_oper_solid[0].transfet_bytes_to_phy_tx = bsp_transfet_bytes_tx_phy0;
    log_oper_solid[0].pull_complete_tx_data = bsp_pull_complete_tx_phy0;
    log_oper_solid[0].phy_baudrate_calibrate = bsp_phy_baudrate_calibrate_phy0;
    log_oper_solid[0].entry_phy_rx = bsp_entry_phy_rx_phy0;
    log_oper_solid[0].entry_phy_tx = bsp_entry_phy_tx_phy0;


 
//-----------------------------------------------------------------------------
}
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
