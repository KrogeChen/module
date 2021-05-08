//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//modbus master solid
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef solid_modbus_master_H
#define solid_modbus_master_H
//------------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++solid+++++++++++++++++++++++++++++++++++++++++
#define max_solid    1
//-----------------------------------------------------------------------------
static mdmRtu_oper_def mdmRtu_oper_solid[max_solid];
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void mdmRtu_solid_cfg(void)
{
//-----------------------------------------------------------------------------
    bsp_phy0_cfg();
    mdmRtu_oper_solid[0].pull_look_for_byte_rx = bsp_look_for_byte_rx_phy0;
    mdmRtu_oper_solid[0].phy_reload = bsp_phy_reload_phy0;
    mdmRtu_oper_solid[0].transfet_bytes_tx_phy = bsp_transfet_bytes_tx_phy0;
    mdmRtu_oper_solid[0].pull_complete_tx_data = bsp_pull_complete_tx_phy0;
    mdmRtu_oper_solid[0].entry_phy_rx = bsp_entry_phy_rx_phy0;
    mdmRtu_oper_solid[0].entry_phy_tx = bsp_entry_phy_tx_phy0;

 
//-----------------------------------------------------------------------------
}
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
