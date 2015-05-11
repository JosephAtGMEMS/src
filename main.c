#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"
#include <example_entry.h>
#include "SensorAPK.h"

extern void wlan_netowrk(void);
extern void console_init(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
        float fdata=0.5f;
        if ( rtl_cryptoEngine_init() != 0 ) {
          DiagPrintf("crypto engine init failed\r\n");
        }

	/* Initialize log uart and at command service */
	console_init();	

	/* pre-processor of application example */
	pre_example_entry();
  
	/* wlan intialization */
#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
	wlan_network();
#endif

	/* Execute application example */
	example_entry();

        xTaskCreate(SensorWORK, (signed portCHAR *)"SensorWORK", 1024, NULL,1,NULL);     
        
    	/*Enable Schedule, Start Kernel*/
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
	#ifdef PLATFORM_FREERTOS
	vTaskStartScheduler();
	#endif
#else
	RtlConsolTaskRom(NULL);
#endif
}
