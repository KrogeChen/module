 //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//用户数据类型重定义
//-------------------------------------------------------------------
#ifndef  snail_data_types
#define  snail_data_types

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//数据重定义
typedef unsigned char        sdt_bool;                     /*布尔量                         */
typedef          char        sdt_string;                   //字符串类型 
typedef unsigned char        sdt_int8u;                    /* Unsigned  8 bit quantity      */
typedef signed   char        sdt_int8s;                    /* Signed    8 bit quantity      */
typedef unsigned short       sdt_int16u;                   /* Unsigned 16 bit quantity      */
typedef signed   short       sdt_int16s;                   /* Signed   16 bit quantity      */
typedef unsigned long        sdt_int32u;                   /* Unsigned 32 bit quantity      */
typedef signed   long        sdt_int32s;                   /* Signed   32 bit quantity      */
typedef unsigned long long   sdt_int64u;                   /* Unsigned 64 bit quantity      */
typedef signed   long long   sdt_int64s;                   /* Signed   64 bit quantity      */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define sdt_true    0x01                               /*布尔运算真值*/
#define sdt_false   0x00                                /*布尔运算假值*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//位运算
#define sdt_set_bit(sram,bits)   (sram|=bits)
#define sdt_clr_bit(sram,bits)   (sram&=~bits)
#define sdt_get_bit(sram,bits)   (sram&bits)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//位定义
#define  sdt_bit0     0x0001
#define  sdt_bit1     0x0002
#define  sdt_bit2     0x0004
#define  sdt_bit3     0x0008
#define  sdt_bit4     0x0010
#define  sdt_bit5     0x0020
#define  sdt_bit6     0x0040
#define  sdt_bit7     0x0080
#define  sdt_bit8     0x0100
#define  sdt_bit9     0x0200
#define  sdt_bit10    0x0400
#define  sdt_bit11    0x0800
#define  sdt_bit12    0x1000
#define  sdt_bit13    0x2000
#define  sdt_bit14    0x4000
#define  sdt_bit15    0x8000
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//文本定义
#define sdt_chars_cr       0x0D  //  \r 回车
#define sdt_chars_lf       0x0A  //  \n 换行
#define sdt_chars_ctrlZ    0x1A  //  CTRL+Z
#define sdt_chars_tLeft    0x3E  //  \>
#define sdt_chars_space    0x20  //  空格
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif	
//++++++++++++++++++++++++++++++++End++++++++++++++++++++++++++++++++++

