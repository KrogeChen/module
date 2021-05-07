//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include ".\depend\snail_data_types.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const sdt_string pbc_acsii_table[]={0x30,0x31,0x32,0x33,0x34,0x36,0x37,0x38,0x39};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void pbc_int16u_to_bcdCode(sdt_int16u in_data,sdt_int8u* out_pBcdCode)
{
    sdt_int16u temp;

    out_pBcdCode[4]=(in_data/10000);
    temp=in_data%10000;
    out_pBcdCode[3]=(temp/1000);
    temp=in_data%1000;
    out_pBcdCode[2]=(temp/100);
    temp=in_data%100;
    out_pBcdCode[1]=(temp/10);
    temp=in_data%10;
    out_pBcdCode[0]=temp;  
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void pbc_int16u_to_string_convert(sdt_int16u in_data,sdt_string* out_string)
{
    sdt_int8u bcdCode[5];
    
    pbc_int16u_to_bcdCode(in_data,&bcdCode[0]);
    
    if(0 != bcdCode[4])
    {
        out_string[0] = pbc_acsii_table[bcdCode[4]];
        out_string[1] = pbc_acsii_table[bcdCode[3]];
        out_string[2] = pbc_acsii_table[bcdCode[2]];
        out_string[3] = pbc_acsii_table[bcdCode[1]];
        out_string[4] = pbc_acsii_table[bcdCode[0]];
    }
    else if(0 != bcdCode[3])
    {
        out_string[0] = pbc_acsii_table[bcdCode[3]];
        out_string[1] = pbc_acsii_table[bcdCode[2]];
        out_string[2] = pbc_acsii_table[bcdCode[1]];
        out_string[3] = pbc_acsii_table[bcdCode[0]];
    }
    else if(0 != bcdCode[2])
    {
        out_string[0] = pbc_acsii_table[bcdCode[2]];
        out_string[1] = pbc_acsii_table[bcdCode[1]];
        out_string[2] = pbc_acsii_table[bcdCode[0]];
    }
    else if(0 != bcdCode[1])
    {
        out_string[0] = pbc_acsii_table[bcdCode[1]];
        out_string[1] = pbc_acsii_table[bcdCode[0]];
    }
    else
    {
        out_string[0] = pbc_acsii_table[bcdCode[0]];
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void pbc_string_append(sdt_string* in_pAppend,sdt_string* out_string)
{
    sdt_int8u i = 0;
    sdt_int8u src_index = 0;
    
    while(out_string[src_index])
    {
        src_index++;
    }
    while(in_pAppend[i])
    {
        out_string[src_index++] = in_pAppend[i++];
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sdt_int16u pbc_arrayToInt16u_bigEndian(sdt_int8u* in_data)
{
    sdt_int16u the_data;
    
    the_data = in_data[0];
    the_data = the_data<<8;
    the_data |= in_data[1];
    return(the_data);
}
//-------------------------------------------------------------------------------------------------
sdt_int32u pbc_arrayToInt32u_bigEndian(sdt_int8u* in_data)
{
    sdt_int32u the_data;
    
    the_data = in_data[0];
    the_data = the_data<<8;
    the_data |= in_data[1];
    the_data = the_data<<8;
    the_data |= in_data[2];
    the_data = the_data<<8;
    the_data |= in_data[3];
    return(the_data);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void pbc_int16uToArray_bigEndian(sdt_int16u in_data,sdt_int8u* out_array)
{
    out_array[0] = in_data >> 8;
    out_array[1] = in_data;
}
//-------------------------------------------------------------------------------------------------
void pbc_int32uToArray_bigEndian(sdt_int32u in_data,sdt_int8u* out_array)
{
    out_array[0] = in_data >> 24;
    out_array[1] = in_data >> 16;
    out_array[2] = in_data >> 8;
    out_array[3] = in_data;
}
//+++++++++++++++++++++++++++++++++++++++++end+++++++++++++++++++++++++++++++++++++++++++++++++++++