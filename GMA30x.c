#include <stdio.h>
#include "GMA30x.h"

//typedef struct sensor_data{
volatile i2c_t *i2c;
volatile int offset[3];
volatile int layout=1;
//}
void Format(char *str,int x,int y,int z)
{
//    printf("x,y,z=%d,%d,%d ",x,y,z);
    int i=0;
    str[i++]=x>0? ' ':'-';if(x<0) x=-x;
    str[i++]=0x30+x/100;x=x-x/100*100;
    str[i++]=0x30+x/10;x=x-x/10*10;
    str[i++]=0x30+x;
    str[i++]=',';
    
    str[i++]=y>0? ' ':'-';if(y<0) y=-y;
    str[i++]=0x30+y/100;y=y-y/100*100;
    str[i++]=0x30+y/10;y=y-y/10*10;
    str[i++]=0x30+y;
    str[i++]=',';

    str[i++]=z>0? ' ':'-';if(z<0) z=-z;
    str[i++]=0x30+z/100;z=z-z/100*100;
    str[i++]=0x30+z/10;z=z-z/10*10;
    str[i++]=0x30+z;
    str[i]=0x00;
}

void InitGMA30x(i2c_t *i2c_master)
{
        i2c=i2c_master;
        /*i2c_write(&i2cmaster, GMA302_Addr, &i2cdatasrc[0], I2C_DATA_LENGTH, 1);
          i2c_read (&i2cmaster, GMA302_Addr, char *data, int length, int stop) {*/
	unsigned char buff[10];
        int err=-1,i;
        printf("Init_GMA302...\n");

	/* 1. Powerdown reset */	
	buff[0] = GMA1302_REG_PD;//reg
        buff[1] = GMA1302_MODE_RESET;//command
	err=i2c_write(i2c,GMA302_Addr,&buff[0] , 2, 1);
        printf(" Powerdown reset ... %d\n",err);
        
	/* 2. check GMA1302_REG_PID(0x00) */
	//i2c_byte_write(i2c,(int)GMA1302_REG_PID);
        buff[0]=GMA1302_REG_PID;
	i2c_write(i2c, GMA302_Addr, &buff[0], 1, 1);
 	err=i2c_read(i2c, GMA302_Addr,&buff[0] , 1, 1);
        printf("GMA1302_REG_PID=%d,err%d\n",buff[0],err);
        for(i=0;i<2;i++)
            printf("0x%x \n",buff[i]);


	/* 3. turn on the offset temperature compensation */
	buff[0] = GMA1302_REG_CONTR3;
	buff[1] = GMA1302_VAL_OFFSET_TC_ON;
	err=i2c_write(i2c, GMA302_Addr, &buff[0], 2, 1);
        printf("3. turn on the offset temperature compensation ... %d\n",err);
	/* 4. turn on the data ready interrupt and configure the INT pin to active high, push-pull type */
	buff[0] = GMA1302_REG_INTCR;
	buff[1] = GMA1302_VAL_DATA_READY_ON;
        err=i2c_write(i2c, GMA302_Addr, &buff[0], 2, 1);
        printf("4. turn on the data ready interrupt ... %d\n",err);
        
        offset[0]=0;
        offset[1]=0;
        offset[2]=0;
        
}

void ReadGMA30x(int *x,int *y,int *z)
{
    unsigned char buff[11]={0,0,0,0,0,0,0,0,0,0,0};
    int err=0,i;
        buff[0]=0x00;
	buff[0] = GMA1302_REG_STADR;// GMA1302_REG_PID;
        //i2c_byte_write(i2c,(int)GMA1302_REG_STADR);
	i2c_write(i2c, GMA302_Addr, &buff[0], 1, 1);
        err=i2c_read(i2c, GMA302_Addr,&buff[0],11,1);
         
        //for(i=0;i<11;i++)
        //    printf("0x%x%x ",(buff[i]&0xf0)>>4,(buff[i]&0xf));
        //printf("\n ");
	//raw data, need layout translate, please reference to datasheet
        
	*x =((int) *(signed short*)&buff[5])>>1;// X axis 
	*y =((int) *(signed short*)&buff[3])>>1;// Y axis 
	*z =-((int) *(signed short*)&buff[7])>>1;// Z axis       
}

void get_offset(int *off)
{
    off[0]=offset[0];
    off[1]=offset[1];
    off[2]=offset[2];
}
void get_Gxyz(int *xyz)
{
    int raw[3];
    ReadGMA30x(&raw[0],&raw[1],&raw[2]);
    xyz[0]=raw[0]-offset[0];
    xyz[1]=raw[1]-offset[1];
    xyz[2]=raw[2]-offset[2];
}

int Calibration(void)
{
    char buf[30];
    const int COUNT=32;
    long int sum[3]={0,0,0};
    int count=0,x,y,z;
    for(count=0;count<COUNT;count++)
    {
        ReadGMA30x(&x,&y,&z);
        //Format(buf,x,y,z);
        //printf("raw=%s \n",buf);     
    }
    for(count=0;count<COUNT;count++)
    {
        ReadGMA30x(&x,&y,&z);
        //Format(buf,x,y,z);
        //printf("raw=%s \n",buf);     
        sum[0]+=x;
        sum[1]+=y;
        sum[2]+=z;
    }
    
    offset[0]=sum[0]/COUNT;
    offset[1]=sum[1]/COUNT;
    offset[2]=sum[2]/COUNT- GMS_DEFAULT_SENSITIVITY;
    //Format(buf,offset[0],offset[1],offset[2]);
    //printf("offset=%s \n",buf);     
  
    return 0; 
}

void get_HP_status(void)
{
    int err;
    char buff[2];
    /* 1. check REG_PID(0x00) */
    i2c_byte_write(i2c,(int)GMA1302_REG_PID);
    err=i2c_read(i2c, GMA302_Addr,&buff[0] , 1, 1);
    printf("GMA1302_REG_PID=%x,err%d\n",buff[0],err);

    buff[0] = GMA1302_REG_CONTR1;//reg
    buff[1] = 0x09;//command
    err=i2c_write(i2c,GMA302_Addr,&buff[0] , 2, 1);
    printf("Set High pass filter off,turn low pass filter on ... %d\n",err);
    
    /* 2. check GMA1302_REG_PID(0x00) */
    i2c_byte_write(i2c,(int)GMA1302_REG_CONTR1);
    err=i2c_read(i2c, GMA302_Addr,&buff[0] , 1, 1);
    printf("GMA1302_REG_CONTR1=%x,err%d\n",buff[0],err);
    
    
}