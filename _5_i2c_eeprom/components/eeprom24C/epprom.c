#include "epprom.h"

void i2c_send_eeprom(uint16_t page, int data)
{
    for(int i=0;i<4;i++)
    {
        BYTE page_upper = page >> 8;
        BYTE page_lower = page & 0xFF;
        data_int_byte temp;
        temp.data_int=data;
        master_write_byte(SLAY_ADDRESS,&page_upper,&page_lower,temp.data_byte+i,1);
        page++;
    }   
}
void i2c_read_eeprom(uint16_t page)
{
   data_int_byte read;
   for(int i=0;i<4;i++)
    {
        BYTE page_upper = page >> 8;
        BYTE page_lower = page & 0xFF;
        master_read_byte(SLAY_ADDRESS,&page_upper,&page_lower,read.data_byte+i,1);
        printf(" data: %02X \n",read.data_byte[i]);
        page++;
    }   
    printf("%d\n",read.data_int);
}