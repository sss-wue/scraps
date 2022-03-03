/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "veneer_table.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_power.h"


#include "qcom_api.h"

#include "iot_wifi.h"
#include "aws_clientcredential.h"


#include "clock_config.h"

#include "iot_mqtt_agent.h"
#include "iot_mqtt.h"
#include "platform/iot_network_freertos.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_SEC_ADDRESS    0x10000000
#define DEMO_NONSEC_ADDRESS 0x20130000
typedef void (*funcptr_t)(char const *s);
#define PRINTF_NSE DbgConsole_Printf_NSE

#define LOGGING_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)
#define LOGGING_TASK_STACK_SIZE (200)
#define LOGGING_QUEUE_LENGTH    (16)
#define TIMEOUT pdMS_TO_TICKS(30000UL)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Global variables
 ******************************************************************************/
uint32_t testCaseNumber;

/*******************************************************************************
 * Code
 ******************************************************************************/

void SystemInit(void)
{
}



void print_string(const char *string)
{
    PRINTF(string);
}

const WIFINetworkParams_t pxNetworkParams = {
    .pcSSID           = clientcredentialWIFI_SSID,
    .ucSSIDLength     = sizeof(clientcredentialWIFI_SSID) - 1,
    .pcPassword       = clientcredentialWIFI_PASSWORD,
    .ucPasswordLength = sizeof(clientcredentialWIFI_PASSWORD) - 1,
    .xSecurity        = clientcredentialWIFI_SECURITY,
};



int initNetwork(void)
{
    WIFIReturnCode_t result;

    PRINTF_NSE(("Starting WiFi...\r\n"));

    result = WIFI_On();
    if (result != eWiFiSuccess)
    {
    	configPRINTF(("Could not enable WiFi, reason %d.\r\n", result));
        return 1;
    }

    PRINTF_NSE(("WiFi module initialized.\r\n"));

    result = WIFI_ConnectAP(&pxNetworkParams);
    if (result != eWiFiSuccess)
    {
    	configPRINTF(("Could not connect to WiFi, reason %d.\r\n", result));
        return 1;
    }

    configPRINTF(("WiFi connected to AP %s.\r\n", pxNetworkParams.pcSSID));

    uint8_t tmp_ip[4] = {0};
    result            = WIFI_GetIP(tmp_ip);

    if (result != eWiFiSuccess)
    {
        configPRINTF(("Could not get IP address, reason %d.\r\n", result));
        return 1;
    }

    configPRINTF(("IP Address acquired %d.%d.%d.%d\r\n", tmp_ip[0], tmp_ip[1], tmp_ip[2], tmp_ip[3]));

    return 0;
}



void main_task(void *pvParameters)
{

    if (SYSTEM_Init() == pdPASS)
    {
        if (initNetwork() != 0)
        {
            configPRINTF(("Network init failed, stopping task.\r\n"));
            vTaskDelete(NULL);
        }
        else
        {

        	if (IotSdk_Init() != true)
        	    {
        	        configPRINTF(("Failed to initialize the common library."));
        	        vTaskDelete(NULL);
        	    }
        	    startMQTT();

        }
    }

    vTaskDelete(NULL);
}


int main(void)
{


    /* set BOD VBAT level to 1.65V */
    POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv, false);

    PRINTF_NSE("Welcome in normal world!\r\n");

    BaseType_t result = 0;
        (void)result;

        /* attach main clock divide to FLEXCOMM0 (debug console) */
        CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

        BOARD_InitBootPins();
        //BOARD_InitBootClocks();
        BOARD_BootClockFROHF96M();
        BOARD_InitDebugConsole();
        CRYPTO_InitHardware();
       // char output[1000];
       // char output2[1000];
       // char output3[1000];
// trustQuerry("test","tets",0.7, &output, 1000);
       // checkRequest(&output2,1000);


        //submitEvidenceVeneer("testID","proverID",&output3,1000);

   //     checkRequest(&output2,1000);

/*
        result =
            xTaskCreate(task_main, "main", TASK_MAIN_STACK_SIZE, task_main_stack, TASK_MAIN_PRIO, &task_main_task_handler);
        assert(pdPASS == result);
*/
        if (xTaskCreate(main_task, "main_task", configMINIMAL_STACK_SIZE * 8, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) //prio was +1
           {
               PRINTF("Main task creation failed!.\r\n");
               while (1)
                   ;
           }

        xLoggingTaskInitialize(LOGGING_TASK_STACK_SIZE, LOGGING_TASK_PRIORITY, LOGGING_QUEUE_LENGTH);

        vTaskStartScheduler();

    while (1)
    {
    }
}

void *pvPortCalloc(size_t xNum, size_t xSize)
{
    void *pvReturn;

    pvReturn = pvPortMalloc(xNum * xSize);
    if (pvReturn != NULL)
    {
        memset(pvReturn, 0x00, xNum * xSize);
    }

    return pvReturn;
}
