#include "i2c_protocol.h"
#include "lcd_16x04.h"
void app_main(void)
{
  i2c_master_init();
  printf("i2c_master_init success \n");
  LCD_INIT();
  printf("lcd_init success \n");
  i2c_send_string("Hung loves Quynh");
  printf("lcd_string success \n");
}
