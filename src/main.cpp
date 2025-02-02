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
#include "control/event_manager.h"

using namespace std;


void load_environment(string env_file) {

    string env_file_string = env_file;

    if (env_file_string.empty())
        env_file_string = SERVICE_ENV_FILE_DEFAULT;
    env_init(env_file_string.c_str());

    cout << "Project Name: " << env_get("PROJECT_NAME", "Unknown") << endl;
}

int main() {

    //load evironment
    load_environment("/Users/phuc/vscode-workspace/FHDO-repos/fhdo-distributed-system/src/.env");
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //system init
    system_init();
    
    spdlog::info("Starting the Truck Platooning system...");

    // create thread that to handle appTasks
    AppStateMachine& appTasks = getAppTasks();

    std::thread app_thread(&AppStateMachine::handleEvent, &appTasks);
    std::thread regularWorkerThread(processCommands, std::ref(regularQueue), std::ref(truckEventFSM));
    std::thread emergencyWorkerThread(processCommands, std::ref(emergencyQueue), std::ref(truckEventFSM));

    // regularWorkerThread.join();
    // emergencyWorkerThread.join();
    app_thread.join();
    return 0;
}