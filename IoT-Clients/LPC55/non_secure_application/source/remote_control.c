/*
 * FreeRTOS Shadow Demo V1.2.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright 2017-2019 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Logging includes. */
#include "iot_logging_task.h"

/* MQTT include. */
#include "iot_mqtt_agent.h"

/* Required to get the broker address and port. */
#include "aws_clientcredential.h"
#include "queue.h"


#include "iot_init.h"

#include "board.h"

#include "veneer_table.h"


#include "tiny-json.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** stack size for task that handles shadow delta and updates
 */
#define DEMO_REMOTE_CONTROL_TASK_STACK_SIZE ((uint16_t)configMINIMAL_STACK_SIZE * (uint16_t)20)

typedef struct
{
    char *pcDeltaDocument;
    uint32_t ulDocumentLength;
    void *xBuffer;
} jsonDelta_t;


/*******************************************************************************
 * Code
 ******************************************************************************/




#define echoCLIENT_ID          ( ( const uint8_t * ) "073B" )

/**
 * @brief The topic that the MQTT client both subscribes and publishes to.
 */
#define ScriptionTopic         ( ( const uint8_t * ) "trustmngt/073B" )
#define PublishTopic         ( ( const uint8_t * )   "trustmngt/Batch" )
/*-----------------------------------------------------------*/

/**
 * @brief Implements the task that connects to the broker and then subscribes
 * to the echoTOPIC_NAME. It then sits forever waiting for the messages sent
 * by the broker.
 *
 * @param[in] pvParameters Parameters passed while creating the task. Unused in our
 * case.
 */
static void prvMQTTConnectAndSubscribeTask( void * pvParameters );

/**
 * @brief Creates an MQTT client and then connects to the MQTT broker.
 *
 * The MQTT broker end point is set by clientcredentialMQTT_BROKER_ENDPOINT.
 *
 * @return pdPASS if everything is successful, pdFAIL otherwise.
 */
static BaseType_t prvCreateClientAndConnectToBroker( void );

/**
 * @brief The callback registered with the MQTT client to get notified when
 * data is received from the broker.
 *
 * @param[in] pvUserData User data as supplied while registering the callback.
 * @param[in] pxCallbackParams Data received from the broker.
 *
 * @return Indicates whether or not we take the ownership of the buffer containing
 * the MQTT message. We never take the ownership and always return eMQTTFalse.
 */
static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxCallbackParams );

/**
 * @brief Subscribes to the echoTOPIC_NAME topic.
 *
 * @return pdPASS if subscribe operation is successful, pdFALSE otherwise.
 */
static BaseType_t prvSubscribe( void );

/*-----------------------------------------------------------*/

static BaseType_t prvPublish( char *, int len) ;

/**
 * @ brief The handle of the MQTT client object used by the MQTT echo demo.
 */
static MQTTAgentHandle_t xMQTTHandle = NULL;

/*-----------------------------------------------------------*/

static BaseType_t prvCreateClientAndConnectToBroker( void )
{
    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn = pdFAIL;
    MQTTAgentConnectParams_t xConnectParameters =
    {
        clientcredentialMQTT_BROKER_ENDPOINT, /* The URL of the MQTT broker to connect to. */
        0,                 					  /* Connection flags. */
        pdFALSE,                              /* Deprecated. */
        clientcredentialMQTT_BROKER_PORT,     /* Port number on which the MQTT broker is listening. */
        echoCLIENT_ID,                        /* Client Identifier of the MQTT client. It should be unique per broker. */
        0,                                    /* The length of the client Id, filled in later as not const. */
        pdFALSE,                              /* Deprecated. */
        NULL,                                 /* User data supplied to the callback. Can be NULL. */
        NULL,                                 /* Callback used to report various events. Can be NULL. */
        NULL,                                 /* Certificate used for secure connection. Can be NULL. */
        0                                     /* Size of certificate used for secure connection. */
    };

    /* Check this function has not already been executed. */
    configASSERT( xMQTTHandle == NULL );

   // char test[8733];

    /* The MQTT client object must be created before it can be used.  The
     * maximum number of MQTT client objects that can exist simultaneously
     * is set by mqttconfigMAX_BROKERS. */
    xReturned = MQTT_AGENT_Create( &xMQTTHandle );

    if( xReturned == eMQTTAgentSuccess )
    {
        /* Fill in the MQTTAgentConnectParams_t member that is not const,
         * and therefore could not be set in the initializer (where
         * xConnectParameters is declared in this function). */
        xConnectParameters.usClientIdLength = ( uint16_t ) strlen( ( const char * ) echoCLIENT_ID );

        /* Connect to the broker. */
        configPRINTF( ( "MQTT attempting to connect to %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
        xReturned = MQTT_AGENT_Connect( xMQTTHandle,
                                        &xConnectParameters,
                                        democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT );

        if( xReturned != eMQTTAgentSuccess )
        {
            /* Could not connect, so delete the MQTT client. */
            ( void ) MQTT_AGENT_Delete( xMQTTHandle );
            configPRINTF( ( "ERROR:  MQTT failed to connect.\r\n" ) );
        }
        else
        {
            configPRINTF( ( "MQTT connected.\r\n" ) );
            xReturn = pdPASS;
        }
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t prvSubscribe( void )
{
    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn;
    MQTTAgentSubscribeParams_t xSubscribeParams;

    /* Setup subscribe parameters to subscribe to echoTOPIC_NAME topic. */
    xSubscribeParams.pucTopic = ScriptionTopic;
    xSubscribeParams.pvPublishCallbackContext = NULL;
    xSubscribeParams.pxPublishCallback = prvMQTTCallback;
    xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) ScriptionTopic );
    xSubscribeParams.xQoS = eMQTTQoS1;

    /* Subscribe to the topic. */
    xReturned = MQTT_AGENT_Subscribe( xMQTTHandle,
                                      &xSubscribeParams,
                                      democonfigMQTT_TIMEOUT );

    if( xReturned == eMQTTAgentSuccess )
    {
        configPRINTF( ( "MQTT  subscribed to %s\r\n", ScriptionTopic ) );
        xReturn = pdPASS;
    }
    else
    {
        configPRINTF( ( "ERROR:  MQTT could not subscribe to %s\r\n", ScriptionTopic ) );
        xReturn = pdFAIL;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t prvPublish( char * pvData, int len ){

	MQTTAgentReturnCode_t xReturned;
	BaseType_t xReturn;
	MQTTAgentPublishParams_t xPublishParams;
	//char test [] = "test";
	xPublishParams.pucTopic = PublishTopic;
	xPublishParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) PublishTopic );
	xPublishParams.xQoS = eMQTTQoS1;
	xPublishParams.pvData = pvData;
	xPublishParams.ulDataLength = len;

	xReturned = MQTT_AGENT_Publish( xMQTTHandle,
            						&xPublishParams,
									democonfigMQTT_TIMEOUT );

	 if( xReturned == eMQTTAgentSuccess )
	    {
	        configPRINTF( ( "Published data successfully to %s\r\n", PublishTopic ) );
	        xReturn = pdPASS;
	    }
	    else
	    {
	        configPRINTF( ( "ERROR: Could not publish to %s\r\n", PublishTopic ) );
	        xReturn = pdFAIL;
	    }

	    return xReturn;
}

/*-----------------------------------------------------------*/

static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxPublishParameters )
{
    /* Remove warnings about the unused parameters. */
    ( void ) pvUserData;

    /* Don't expect the callback to be invoked for any other topics. */
    configASSERT( strlen( ( const char * ) ScriptionTopic ) == ( size_t ) ( pxPublishParameters->usTopicLength ) );
    configASSERT( memcmp( pxPublishParameters->pucTopic, ScriptionTopic, ( size_t ) ( pxPublishParameters->usTopicLength ) ) == 0 );

    /* Print the received message. */
    //configPRINTF( ( "Received: %.*s.\r\n", pxPublishParameters->ulDataLength, pxPublishParameters->pvData ) );

     //expected size 1407
    char testmsg[1407];
    char * buf3 = "e8b77a30c1ad30c560ae5342edf8111ec6449b24758e19e910b488cc4b369970763add25241f5bb4737eff9d03698c70a4a1114ba9233a77ddf7257ba79237c3";
    submitEvidenceVeneer(&buf3,&testmsg,1407);
    configPRINTF( ( "send evidence...\r\n" ) );
    prvPublish(&testmsg,1407);
    /* Returning eMQTTFalse tells the MQTT agent that the ownership
     * of the buffer containing the message lies with the agent and
     * it is responsible for freeing the buffer. */
    return eMQTTFalse;
}


/*-----------------------------------------------------------*/


static void prvMQTTConnectAndSubscribeTask( void * pvParameters )
{
    BaseType_t xReturned;
    const TickType_t xFiveSeconds = pdMS_TO_TICKS( 5000UL );

    /* Avoid compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Create the MQTT client object and connect it to the MQTT broker. */
    xReturned = prvCreateClientAndConnectToBroker();

    if( xReturned == pdPASS )
    {
        /* Subscribe to the echo topic. */
        xReturned = prvSubscribe();

    }
    if( xReturned == pdPASS) {
    	  char testmsg[1407];
    	 // char buf[7] = {'t', 'r', 'u', 's', 't', 'o', 'r'};
    	 // char buf2[7] = {'t', 'r', 'u', 's', 't', 'e', 'e'};

    	  checkRequest(&testmsg,1407);

    	  configPRINTF( ( "send check request...\r\n" ) );
    	  enum { MAX_FIELDS = 8 };
    	  json_t pool[ MAX_FIELDS ];

    	  json_t const* message = json_create( testmsg, pool, MAX_FIELDS );
          // if ( message == NULL ) return EXIT_FAILURE;
    	  // prvPublish(&testmsg,1407);
    	  configPRINTF( ( "2 send check request...\r\n" ) );
    	  prvPublish(&message,1407);

    }
    /* MQTT client is now connected to a broker. Keep waiting
     * for the messages received from the broker. */
    for( ; ; )
    {
       // configPRINTF( ( "Waiting for the messages from the broker...\r\n" ) );

        /* Five seconds delay between loops. */
        vTaskDelay( xFiveSeconds );
    }
}
/*-----------------------------------------------------------*/

void startMQTT( void )
{
    configPRINTF( ( "Creating MQTT Echo Task...\r\n" ) );

    /* Create the task that connects to the MQTT broker and waits
     * for the messages from the broker. */
    ( void ) xTaskCreate( prvMQTTConnectAndSubscribeTask,      /* The function that implements the task. */
                          "MQTTtask",                          /* The name to assign to the task being created. */
                          democonfigMQTT_ECHO_TASK_STACK_SIZE, /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          NULL,                                /* The task parameter is not being used. */
                          democonfigMQTT_ECHO_TASK_PRIORITY,   /* The priority at which the task being created will run. */
                          NULL );                              /* Not storing the task's handle. */
}
