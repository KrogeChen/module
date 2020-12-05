//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "chip_select.h"
#include ".\snail_data_types.h"
#include "sysctrl.h"
#include "intrinsics.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(stm32f103rct6)

#elif defined(stm8l052r8t6)

#elif defined(hc32l136k8ta)
#include ".\bsp_system_tick_hc32l136k8ta.h"
#elif defined(hc32l176kata)
#include ".\bsp_system_tick_hc32l176kata.h"
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++