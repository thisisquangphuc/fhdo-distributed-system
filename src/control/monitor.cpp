/*
 * Author : Phuc Le
 *
 * Created on Mon Jan 06 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */
// #include <unistd.h>

// #define SAFE_DISTANCE 10
// #define BRAKING_DISTANCE 7

// void send_braking_signal();
// double get_leading_pos();
// double get_following_pos();

// double calculate_distance() {
//     // Assume we have 2 trucks, will develop more trucks logic later
//     double leading_pos = get_leading_pos();
//     double following_pos = get_following_pos();

//     return following_pos - leading_pos;
// }

// int is_braking_needed(double dis) {
//     return (dis < BRAKING_DISTANCE);
// }

// int is_connection_lost() {
//     // Check connection timeout or connection lost
//     //return false 
//     return 1;
// }

// void* monitoring() {
//     double distance = calculate_distance();
//     while (1) {
//         if (distance < SAFE_DISTANCE) {
//             // printf("Distance too close.\n");
//             if (is_braking_needed(distance)) {
//                 // printf("Send braking signals");
//                 send_braking_signal();
//                 //slow down 
//             }
//         } else {
//             // Do nothing

//         }
//         sleep(1);
//     }
//     return;
// } 