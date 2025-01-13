/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

// #include "comm_manager.h"
// #include <MQTTClient.h>

// #define ADDRESS         "tcp://localhost:1883"   
// #define CLIENTID        "TruckPlatoonClient"    
// #define EMGENCY_TOPIC   "truck/emergency"   
// #define QOS             1                     
// #define TIMEOUT         10000L                

// void messageArrived(MessageData* data) {
//     printf("Message arrived: %s\n", data->message->payload);
//     msg_process(data->message->payload);
// }

// int mqtt_init() {
//     MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//     int rc;

//     // Create the MQTT client instance
//     MQTTClient_create(&client, ADDRESS, CLIENTID,
//                       MQTTCLIENT_PERSISTENCE_NONE, NULL);

//     // Set the message arrival callback function
//     MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

//     // Connect to the broker
//     conn_opts.keepAliveInterval = 20;
//     conn_opts.cleansession = 1;
//     rc = MQTTClient_connect(client, &conn_opts);
//     if (rc != MQTTCLIENT_SUCCESS) {
//         printf("Failed to connect, return code %d\n", rc);
//         return rc;
//     }
//     printf("Connected to MQTT broker at %s\n", ADDRESS);

//     // Subscribe to a topic
//     rc = MQTTClient_subscribe(client, TOPIC, QOS);
//     if (rc != MQTTCLIENT_SUCCESS) {
//         printf("Failed to subscribe, return code %d\n", rc);
//         return rc;
//     }
//     printf("Subscribed to topic %s\n", TOPIC);

//     return MQTTCLIENT_SUCCESS;
// }

// void mqtt_cleanup() {
//     MQTTClient_disconnect(client, 1000);
//     MQTTClient_destroy(&client);
//     printf("Disconnected and cleaned up MQTT client.\n");
// }

// int publish_msg(const char* topic, const char* message) {
//     int rc;
//     char truck_topic[100];
//     MQTTClient_message pub_message = MQTTClient_message_initializer;

//     // Set the message payload
//     pub_message.payload = (void*) message;
//     pub_message.payloadlen = strlen(message);
//     pub_message.qos = QOS;
//     pub_message.retained = 0;

//     // Publish message
//     rc = MQTTClient_publishMessage(client, topic, &pub_message, NULL);
//     if (rc != MQTTCLIENT_SUCCESS) {
//         printf("Failed to publish message to %s, return code %d\n", topic, rc);
//     } else {
//         printf("Message sent to %s: %s\n", topic, message);
//     }

//     return rc;
// }
