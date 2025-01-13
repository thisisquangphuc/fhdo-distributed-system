/*
 * Author : Phuc Le
 *
 * Created on Mon Jan 06 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

// #include "communication/comm_manager.h"
// #include "utils/config.h"

// int send_braking_signal() {
//     // Send braking signal to all trucks
//     return 0;
// }

// void emergency_braking_signal(int message) {
//     //Compose topic using truck_id
//     char topic[256];
//     sprintf(topic, "truck/emergency");
//     return publish_msg(topic, message);
// }

// void msg_process(const char* msg) {
//     //Process 

// }

// int send_msg(const char* truck_id, const char* message) {
//     //Compose topic using truck_id
//     char topic[256];
//     sprintf(topic, "truck/%s", truck_id);
//     return publish_msg(topic, message);
// }

// void* comm_hanlder() {
//     while (1) {
//         // Get msg from queue then send to trucks
        

//         sleep(1);
//     }
//     return;
// }