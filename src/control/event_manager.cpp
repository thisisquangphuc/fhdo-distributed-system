/*
 * Author : Phuc Le
 *
 * Created on Mon Jan 06 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include "event_manager.h"

void TruckStateMachine::handleIdle(const TruckMessage& msg) {
    // Do nothing at this moment
}

void TruckStateMachine::handleAuthen(const TruckMessage& msg) {
    std::lock_guard<std::mutex> lock(databaseMutex);
    // get auth_key from msg
    std::string auth_key = "1234";//msg.getAuthKey();
    // check auth_key
    std::string expectedToken = env_get("AUTH_KEY", "");
    // if (truck_id_map.find(authKey) == truck_id_map.end()) {
    //     return R"({"cmd":"auth_response","status":"FAILED","message":"Invalid authentication key"})";
    // }

    if (auth_key == expectedToken) {
        //gen truck id 
        std::string truck_id = generateTruckID();
        // update truck id to hashmap
        // truck_id_map[truck_id] = truck_id;
    }
}

void TruckStateMachine::handleJoin(const TruckMessage& msg) {

}
void TruckStateMachine::handleLeave(const TruckMessage& msg) {

}
void TruckStateMachine::handleCommunicate(const TruckMessage& msg) {

}
void TruckStateMachine::handleEmergency(const TruckMessage& msg) {

}



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