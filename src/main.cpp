/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include "utils/env.h"
#include "utils/config.h"
#include "app.h"

using namespace std;

AppStateMachine app_tasks;
TruckState app_state = INIT;

TruckState get_current_state() {
    return app_state;
}

void load_environment(string env_file) {

    string env_file_string = env_file;

    if (env_file_string.empty())
        env_file_string = SERVICE_ENV_FILE_DEFAULT;
    env_init(env_file_string.c_str());

    cout << "Project Name: " << env_get("PROJECT_NAME", "Unknown") << endl;
}

void AppTask() {
    vector<string> pre_shared_key;
    // while (true)
    // {
    //     TruckState current_state = get_current_state();
    //     switch (current_state) {
    //         case INIT:
    //             // Generate pre-shared keys for number of trucks
    //             int num_trucks = env_get_int("NUM_TRUCKS", TOTAL_TRUCKS);
    //             // store keys in a list
    //             pre_shared_key = generate_pre_shared_key(num_trucks);
    //             //print pre-shared keys to terminal with key map
    //             spdlog::info("Pre-shared key: {}", pre_shared_key);
    //             // Set current state to IDLE
    //             app_state = IDLE;
    //             break;
    //         case IDLE:
    //             // Check all conditions that are ready to start 
    //             break;
    //         case NORMAL_OPERATION:
    //             // Handle normal operation:
    //             // 1. Join platoon 
    //             // 2. Leave platoon
    //             // 3. Obstacle Avoidance
    //             // 4. Maintain distance
    //             printf("Truck is in NORMAL_OPERATION state.\n");
    //             // #pragma omp parallel sections
    //             // {
    //             //     #pragma omp section
    //             //     {
    //             //         printf("Truck is joining the platoon...\n");
    //             //         if (is_truck_joining_needed()) {
    //             //             char* truck_id = get_truck_id();
    //             //             join_platoon(truck_id);
    //             //         }
    //             //     }

    //             //     #pragma omp section
    //             //     {
    //             //         printf("Truck is leaving the platoon...\n");
    //             //         if (is_truck_joining_needed()) {
    //             //             char* truck_id = get_truck_id();
    //             //             leave_platoon(truck_id);
    //             //         }
    //             //     }

    //             //     #pragma omp section
    //             //     {
    //             //         printf("Truck is avoiding obstacles...\n");
    //             //         if (is_obstacle_avoidance_needed()) {
    //             //             avoid_obstacles();
    //             //         }
    //             //     }
    //             // }

    //             break;
    //         case SPEED_UP:
    //         //     printf("Truck is SPEEDING UP to close the gap.\n");
    //         //     send_speedup_signal();
    //             break;
    //         case SLOW_DOWN:
    //         //     printf("Truck is SLOWING DOWN to maintain distance.\n");
    //         //     send_slowdown_signal();
    //             break;
    //         case EMERGENCY_BRAKE:
    //             printf("Truck is in EMERGENCY_BRAKE state.\n");
                
    //             break;
    //         case CONNECTION_LOST:
    //             printf("Truck is in CONNECTION_LOST state.\n");
    //             // ping_trucks(truck_id);
    //             break;
    //         default:
    //             break;
    //     }
    //     std::this_thread::sleep_for (std::chrono::seconds(1));
    // }
}

int main() {

    //load evironment
    load_environment("/Users/phuc/vscode-workspace/FHDO-repos/fhdo-distributed-system/src/.env");
    
    //system init
    system_init();
    
    spdlog::info("TEST - Starting the Truck Platooning system...");

    TruckMessage msg;
    // Set outgoing payload values
    msg.setTruckID("truck123");
    msg.setCommand("join");
    msg.setLocation(37.7749, -122.4194);
    msg.setDistances(15, 10, 20);
    msg.setSpeed(80);

    // Build and print outgoing payload
    std::cout << "Outgoing Payload:" << std::endl;
    std::cout << msg.buildPayload() << std::endl;
    
    // create thread that run app tasks

    // std::thread app_thread(AppTask);
    std::thread app_thread(&AppStateMachine::handleEvent, &app_tasks, msg);

    app_thread.join();
    return 0;
}