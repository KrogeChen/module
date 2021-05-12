//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "chip_select.h"
#include ".\snail_data_types.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(stm32f103rct6)

#elif defined(stm8l052r8t6)

#elif defined(hc32l136k8ta)
#include ".\bsp_system_tick_hc32l136k8ta.h"
#elif defined(hc32l176kata)
#include ".\bsp_system_tick_hc32l176kata.h"
#elif defined(n32g455rcl7)
#include ".\bsp_system_tick_n32g455rcl7.h"
#elif defined(fm33lc025n)
#include ".\bsp_system_tick_fm33lc025n.h"

#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++