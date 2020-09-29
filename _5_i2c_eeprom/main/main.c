/* 
    The ESP32 chip features 40 physical GPIO pads
    NOTES:
        - Note that GPIO6-11 are usually used for SPI flash  
        - GPIO34-39 can only be set as input mode and do not have 
        software pullup or pulldown functions.
*/
#include "epprom.h"
#include "i2c_protocol.h"

void app_main(void)
{
  i2c_master_init();
  i2c_send_eeprom(1,1234);
  i2c_read_eeprom(1);
}
