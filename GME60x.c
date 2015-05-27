#include <stdio.h>
#include "GMA30x.h"

//typedef struct sensor_data{
volatile i2c_t *i2ce;
AKMPRMS pmem;
AKMPRMS *prms=(AKMPRMS*)&pmem;
//}

void InitGME60x(i2c_t *i2c_master)
{
        unsigned char buff[11]={0,0,0,0,0,0,0,0,0,0,0};
        int err=0,i;

        i2ce=i2c_master;
        printf("Init_GME60x(M sensor)...size of pmem=%d\n",sizeof(pmem));
        
//	i2c_write(i2c, GMA302_Addr, &buff[0], 1, 1);
//        err=i2c_read(i2c, GMA302_Addr,&buff[0],11,1);

	AKFS_PATNO	pat=PAT1;
	unsigned char	regs[3];

        buff[0]=AK09911_REG_WIA1;
        i2c_write(i2ce, M_sensor_Addr, &buff[0], 1, 1);
        err=i2c_read(i2ce, M_sensor_Addr,&buff[0],AKM_SENSOR_INFO_SIZE,1);
	printf("\n\nCampany ID =0x%x Device ID=0x%x\n\n",buff[0],buff[1]);

	//Get sense_conf 3 bytes, reg 60~62H
	//(1) Set FUSE access mode
        buff[0]=AK09911_REG_CNTL2;
        buff[1]=0x1f;//11111 FUSE ROM access mode
        i2c_write(i2ce, M_sensor_Addr, &buff[0], 2, 1);
        
        
	//(2) Read AK09911_FUSE_ASAX~AK09911_FUSE_ASAZ 3 bytes;
        buff[0]=AK09911_FUSE_ASAX;
        i2c_write(i2ce, M_sensor_Addr, &buff[0], 1, 1);
        err=i2c_read(i2ce, M_sensor_Addr,&buff[0],AKM_SENSOR_INFO_SIZE,1);
        regs[0]=buff[0];
        regs[1]=buff[1];
        regs[2]=buff[2];
	printf("Asax,Asay,Asaz=0x%x,0x%x,0x%x\n",buff[0],buff[1],buff[2]);//asax,asay,asaz
        
	//(3)Set PowerDown mode
        buff[0]=AK09911_FUSE_ASAX;
	buff[1]=0x00;//00000 Power Down mode
        i2c_write(i2ce, M_sensor_Addr, &buff[0], 1, 1);

	if (AKFS_Init(prms, pat, regs) != AKM_SUCCESS) {
		printf("AKFS_Init ERROR\n");
		return;
	}
	printf("prms->fv_hs.v=%f,%f,%f\n",prms->fv_hs.v[0],prms->fv_hs.v[1],prms->fv_hs.v[2]);
	printf("prms->i8v_asa=%d,%d,%d\n",prms->i8v_asa.v[0],prms->i8v_asa.v[1],prms->i8v_asa.v[2]);
	printf("prms->e_hpat=%d\n",prms->e_hpat);

	//
	if (AKFS_Start(prms, CSPEC_SETTING_FILE) != AKM_SUCCESS) {
		printf("AKFS_Start ERROR\n");
		return;
	}
	//00110 Continuous measurement mode 4

        buff[0]=AK09911_REG_CNTL2;
        buff[1]=0x06;//11111 FUSE ROM access mode
        i2c_write(i2ce, M_sensor_Addr, &buff[0], 2, 1);
    
        
}

void ReadGME60x(int *x,int *y,int *z)
{
    unsigned char buff[11]={0,0,0,0,0,0,0,0,0,0,0};
    int err=0,i;
        buff[0]=0x00;
	buff[0] = GMA1302_REG_STADR;// GMA1302_REG_PID;
        i2c_byte_write(i2ce,(int)GMA1302_REG_STADR);
	err=i2c_read(i2ce, GMA302_Addr,&buff[0],11,1);
         
        //for(i=0;i<11;i++)
        //    printf("0x%x%x ",(buff[i]&0xf0)>>4,(buff[i]&0xf));
        //printf("\n ");
	//raw data, need layout translate, please reference to datasheet
        
	*x =-((int) *(signed short*)&buff[5])>>1;// X axis 
	*y =-((int) *(signed short*)&buff[3])>>1;// Y axis 
	*z =((int) *(signed short*)&buff[7])>>1;// Z axis       
}

