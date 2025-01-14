/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include <iostream>
#include <cstring>
#include "utils/env.h"
#include "utils/config.h"
#include "app.h"

using namespace std;
typedef enum {
    IDLE,
    NORMAL_OPERATION,
    SPEED_UP,
    SLOW_DOWN,
    EMERGENCY_BRAKE,
    CONNECTION_LOST
} TruckState;

TruckState current_state = IDLE;

TruckState get_current_state() {
    return current_state;
}

void load_environment(string env_file) {

    string env_file_string = env_file;

    if (env_file_string.empty())
        env_file_string = SERVICE_ENV_FILE_DEFAULT;
    env_init(env_file_string.c_str());

    cout << "Project Name: " << env_get("PROJECT_NAME", "Unknown") << endl;
}

void AppTask() {

    switch (current_state) {
        case IDLE:
            // Check all conditions that are ready to start 
            break;
        case NORMAL_OPERATION:
            // Handle normal operation:
            // 1. Join platoon 
            // 2. Leave platoon
            // 3. Obstacle Avoidance
            // 4. Maintain distance
            printf("Truck is in NORMAL_OPERATION state.\n");
            // #pragma omp parallel sections
            // {
            //     #pragma omp section
            //     {
            //         printf("Truck is joining the platoon...\n");
            //         if (is_truck_joining_needed()) {
            //             char* truck_id = get_truck_id();
            //             join_platoon(truck_id);
            //         }
            //     }

            //     #pragma omp section
            //     {
            //         printf("Truck is leaving the platoon...\n");
            //         if (is_truck_joining_needed()) {
            //             char* truck_id = get_truck_id();
            //             leave_platoon(truck_id);
            //         }
            //     }

            //     #pragma omp section
            //     {
            //         printf("Truck is avoiding obstacles...\n");
            //         if (is_obstacle_avoidance_needed()) {
            //             avoid_obstacles();
            //         }
            //     }
            // }

            break;
        case SPEED_UP:
        //     printf("Truck is SPEEDING UP to close the gap.\n");
        //     send_speedup_signal();
            break;
        case SLOW_DOWN:
        //     printf("Truck is SLOWING DOWN to maintain distance.\n");
        //     send_slowdown_signal();
            break;
        case EMERGENCY_BRAKE:
            printf("Truck is in EMERGENCY_BRAKE state.\n");
            
            break;
        case CONNECTION_LOST:
            printf("Truck is in CONNECTION_LOST state.\n");
            // ping_trucks(truck_id);
            break;
    }

}

int main() {

    //load evironment
    load_environment("/Users/phuc/vscode-workspace/FHDO-repos/fhdo-distributed-system/src/.env");
    
    //system init
    system_init();
    
    spdlog::info("TEST - Starting the Truck Platooning system...");
    
    return 0;
}