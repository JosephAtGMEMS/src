#include "SensorAPK.h"
#include <stdio.h>
#include "GMA30x.h"
#include "timer_api.h"

#if defined (__ICCARM__)
i2c_t   i2cmaster;
#else
volatile i2c_t   i2cmaster;
#endif


void magCaliTask(void *p)
{
}

void FusionTask(void *p)
{
    char str[80]; 
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
      getgM(str);
      printf("g,M=%s\n",str);    
      vTaskDelayUntil( &xLastWakeTime, ( 20 / portTICK_RATE_MS ) );
    }
}
void SensorWORK(void *p)
{
    uint32_t hid=0;

    int i=0;
    i2c_init(&i2cmaster, MBED_I2C_MTR_SDA ,MBED_I2C_MTR_SCL);
    i2c_frequency(&i2cmaster,MBED_I2C_BUS_CLK);
     
    InitGMA30x(&i2cmaster);
    InitGME60x(&i2cmaster);
    printf("init completed\n");
    get_HP_status();
    Calibration();  

    //xTaskCreate(magCaliTask, (signed portCHAR *)"magCaliTask", 1024, NULL,2,NULL);     

    xTaskCreate(FusionTask, (signed portCHAR *)"FusionTask", 1024, NULL,3,NULL);     
    vTaskDelete(NULL);    
}
/*
void FusionTask(void *p)
{
    char str[80]; 
    portTickType xLastWakeTime,t0,t1;
    xLastWakeTime = xTaskGetTickCount();
    volatile int g[3],M[3];
    char buf[30];
    while(1)
    {
      t0 = xTaskGetTickCount();
     // getgM(str);
      get_Gxyz(&g[0]);
      get_Mxyz(&M[0]);
      Format(buf,g[0],g[1],g[2]); strcpy(str,buf);
      Format(buf,M[0],M[1],M[2]); strcat(str,",");strcat(str,buf);
      printf("g,M=%s t0,t1=%d,%d\n",str,t0,t1);    
      t1 = xTaskGetTickCount();
     
      
      vTaskDelayUntil( &xLastWakeTime, ( 20 / portTICK_RATE_MS ) );
    }
}*/
/*
gtimer_t my_timer0;
gtimer_t my_timer1;
volatile int timeup0=0;
volatile int timeup1=0;
void timer0_timeout_handler(uint32_t id)
{
    timeup0=1;
}
void timer1_timeout_handler(uint32_t id)
{
    timeup1=1;
}
    gtimer_init(&my_timer0, TIMER0);
    gtimer_start_periodical(&my_timer0, 125000, (void*)timer1_timeout_handler, (uint32_t)&hid);
    gtimer_init(&my_timer1, TIMER1);
    gtimer_start_periodical(&my_timer1, 20000, (void*)timer1_timeout_handler, (uint32_t)&hid);


*/