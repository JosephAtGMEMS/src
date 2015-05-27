#include "SensorAPK.h"
#include <stdio.h>
#include "GMA30x.h"

#if defined (__ICCARM__)
i2c_t   i2cmaster;
#else
volatile i2c_t   i2cmaster;
#endif


void SensorWORK(void *p)
{
    int    g[3];
    char buf[30];
    int i=0;
    i2c_init(&i2cmaster, MBED_I2C_MTR_SDA ,MBED_I2C_MTR_SCL);
    i2c_frequency(&i2cmaster,MBED_I2C_BUS_CLK);
     
    InitGMA30x(&i2cmaster);
    InitGME60x(&i2cmaster);
    printf("init completed\n");
    get_HP_status();
    Calibration();   
    while(i++<10)
    {
      get_Gxyz(&g[0]);
      Format(buf,g[0],g[1],g[2]);
      printf("(x,y,z)=%s\n",buf);     
    }
    
}
