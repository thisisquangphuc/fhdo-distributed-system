// /*
//  * Author : Phuc Le
//  *
//  * Created on Mon Jan 06 2025
//  *
//  * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
//  */

// typedef enum {
//     IDLE,
//     NORMAL_OPERATION,
//     SPEED_UP,
//     SLOW_DOWN,
//     EMERGENCY_BRAKE,
//     CONNECTION_LOST
// } TruckState;

// TruckState current_state = IDLE;

// TruckState get_current_state() {
//     return current_state;
// }

// void change_state_to(TruckState state) {
//     current_state = state;
// }

// void join_platoon(char* truck_id) {
//     // slow down all trucks in platoon then allow others to join at the tail 
//     // Set timeput and change state to SLOW_DOWN 
    
// }

// void* event_handler() {
//     while (1) {

//         switch (current_state) {
//             case IDLE:
//                 // Check all conditions that are ready to start 


//                 break;
//             case NORMAL_OPERATION:
//                 // Handle normal operation:
//                 // 1. Join platoon 
//                 // 2. Leave platoon
//                 // 3. Obstacle Avoidance
//                 // 4. Maintain distance
//                 printf("Truck is in NORMAL_OPERATION state.\n");
//                 #pragma omp parallel sections
//                 {
//                     #pragma omp section
//                     {
//                         printf("Truck is joining the platoon...\n");
//                         if (is_truck_joining_needed()) {
//                             char* truck_id = get_truck_id();
//                             join_platoon(truck_id);
//                         }
//                     }

//                     #pragma omp section
//                     {
//                         printf("Truck is leaving the platoon...\n");
//                         if (is_truck_joining_needed()) {
//                             char* truck_id = get_truck_id();
//                             leave_platoon(truck_id);
//                         }
//                     }

//                     #pragma omp section
//                     {
//                         printf("Truck is avoiding obstacles...\n");
//                         if (is_obstacle_avoidance_needed()) {
//                             avoid_obstacles();
//                         }
//                     }
//                 }

//                 break;
//             // case SPEED_UP:
//             //     printf("Truck is SPEEDING UP to close the gap.\n");
//             //     send_speedup_signal();
//             //     break;
//             // case SLOW_DOWN:
//             //     printf("Truck is SLOWING DOWN to maintain distance.\n");
//             //     send_slowdown_signal();
//             //     break;
//             case EMERGENCY_BRAKE:
//                 printf("Truck is in EMERGENCY_BRAKE state.\n");
                
//                 break;
//             case CONNECTION_LOST:
//                 printf("Truck is in CONNECTION_LOST state.\n");
//                 ping_trucks(truck_id);
//                 break;
//         }
//         sleep(1);
//     }
//     return;
// }