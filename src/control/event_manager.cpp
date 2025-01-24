/*
 * Author : Phuc Le
 *
 * Created on Mon Jan 06 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include "event_manager.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>
#include "app.h"

// StateMachine stateMachine
void processCommands(MsgQueue& queue, TruckEventFSM& stateMachine) {
    while (true) {
        try {
            std::string command = queue.dequeueCommand();
            if (command.empty()) break; // Stop if queue is stopped
            std::cout << "Processing Command: " << command << std::endl;
            // Parse the command
            TruckMessage truckMsg(command);
            string cmdName = truckMsg.getCommand();
            stateMachine.setState(cmdName);
            stateMachine.handleEvent(truckMsg);
        } catch (const std::exception& e) {
            std::cerr << "Error processing command: " << e.what() << std::endl; 
        }
    }
}

void TruckEventFSM::handleIdle(const TruckMessage& msg){
    // 
}
void TruckEventFSM::handleJoin(const TruckMessage& msg){
    spdlog::info("Process join request ...");
    int cli_socket = -1;
    try
    {
        // Get truck id
        string id = msg.getTruckID();
        // Get truck order from TruckManager
        TruckManager& truckManager = TruckManager::getInstance();
        cli_socket = truckManager.getSocketId(id); // This may throw

        // Check available slot
        if (!truckManager.isSlotAvailable()) {
            // No available slot
            json error = {{"error", "No available slot"}};
            PlatoonServer::sendResponse(cli_socket, error);
            return;
        }

        // Send synchronization data such as location and speed, etc.
        PlatoonDataManager& platoon_data = PlatoonDataManager::getInstance();
        json data = platoon_data.getPlatoonDataJSON();

        // send command "join_accepted" and data to client
        TruckMessage rspMsg;
        rspMsg.setCommand("join_accepted");
        PlatoonServer::sendResponse(cli_socket, rspMsg.buildPayload(data));

    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << '\n';
        spdlog::error("Truck ID not found.");
        //send error response
        json error = {{"error", "Truck ID not found"}};
        PlatoonServer::sendResponse(cli_socket, error);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

void TruckEventFSM::handleLeave(const TruckMessage& msg){
    // 
    spdlog::info("Process leave request ...");
    int cli_socket = -1;
    try
    {
        TruckManager& truckManager = TruckManager::getInstance();
        // Get truck id
        string id = msg.getTruckID();
        int leaveSocket = truckManager.getSocketId(id);
        
        //Check command
        if (msg.getCommand() == "leave_done") {
            //Remove the leaving truck from the platoon
            spdlog::info("Truck {} leave done, remove from platoon", id);
            truckManager.removeTruck(id);
            
            // Send "sync" command to all trucks
            PlatoonDataManager& platoon_data = PlatoonDataManager::getInstance();
            // increase speed
            platoon_data.updateTruckField(&PlatoonData::speed, platoon_data.getPlatoonData().speed + 15);
            // decrease brake force
            platoon_data.updateTruckField(&PlatoonData::brakeForce, platoon_data.getPlatoonData().brakeForce - 0.2);

            json data = platoon_data.getPlatoonDataJSON();
            TruckMessage rspMsg;
            rspMsg.setCommand("sync");

            vector<pair<string, int>> allTrucks = truckManager.getTrucks();
            for (const auto& truck : allTrucks) {
                cli_socket = truckManager.getSocketId(truck.first);
                PlatoonServer::sendResponse(cli_socket, rspMsg.buildPayload(data));
            }

            return;
        }
        //Check if there are any emergency signal
        if (isEmergencyEnabled()) {
            // Posepone leave request
            json error = {{"error", "Emergency signal is being processed. Leave request is postponed."}};
            PlatoonServer::sendResponse(cli_socket, error);
            return;
        }
        spdlog::info("No emergency signal!");
        // Get trucks behind the leaving truck
        // vector<pair<string, int>> trucksBehind = truckManager.getTrucksBehind(id);
        //Send slowdown signal to trucks behind the leaving truck
        // for (const auto& truck : trucksBehind) {
        //     cli_socket = truckManager.getSocketId(truck.first);
        //     json data = {{"command", "slow_down"}};
        //     PlatoonServer::sendResponse(cli_socket, data);
        // }

        //get info of all trucks 
        vector<pair<string, int>> allTrucks = truckManager.getTrucks();
        //Send slowdown signal to all trucks in the platoon 

        PlatoonDataManager& platoon_data = PlatoonDataManager::getInstance();
        // decrease speed
        platoon_data.updateTruckField(&PlatoonData::speed, platoon_data.getPlatoonData().speed - 15);
        // increase brake force
        platoon_data.updateTruckField(&PlatoonData::brakeForce, platoon_data.getPlatoonData().brakeForce + 0.2);

        json data = platoon_data.getPlatoonDataJSON();

        TruckMessage rspMsg;
        rspMsg.setCommand("slow_down");
        string payload = rspMsg.buildPayload(data);
        for (const auto& truck : allTrucks) {
            if (truck.first == id) {
                // skip the leaving truck
                continue;
            }
            cli_socket = truck.second;
            PlatoonServer::sendResponse(cli_socket, payload);
        }

        //Response to leaving truck
        rspMsg.setCommand("leave_start");
        payload = rspMsg.buildPayload(data);
        PlatoonServer::sendResponse(leaveSocket, payload);

    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << '\n';
        spdlog::error("Truck ID not found or invalid.");
        //send error response
        json error = {{"error", "Truck ID not found or invalid."}};
        PlatoonServer::sendResponse(cli_socket, error);

    } catch (const std::exception& e) {
        spdlog::error("Error during processing leave request: {}", e.what());
    }
}
void TruckEventFSM::handleCommunicate(const TruckMessage& msg){
    // Proces status messages from following trucks
    if (msg.getCommand() == "status") {
        // Process status message
        //[TODO] Get distances and speeds from the message and calculate the new status based on it
        // Get front and back distances in distance field of the message along with "front" and "back"

        double frontDistance = msg.getFrontDistance();
        double backDistance = msg.getBackDistance();

        // if these distances are in range of dangerous distances, set the status to "danger"
        //get safe distance from env
        double safeDistance = env_get_double("SAFE_DISTANCE", TRUCK_SAFE_DISTANCE);
        if (frontDistance < (safeDistance + 5) || backDistance < (safeDistance + 5)) {
            // Enable Emergency signal and change the state to "emergency"
            // setEmergencyEnabled(true);

            // Switch to emergency state
            AppStateMachine& appTasks = getAppTasks();
            appTasks.switchToEmergency();
        } 
        //Further processing

    }
    
}
void TruckEventFSM::handleEmergency(const TruckMessage& msg){
    // Get the emergency message, parse it and send it to all trucks
    // Decison making logic for emergency response

}

bool TruckEventFSM::Auth(const TruckMessage& msg, string truckID) {
    // [DONE] Handled in PlatoonServer
    return true;
}

void TruckEventFSM::handleExternal(const TruckMessage& msg){
    // Get command
    string command = msg.getCommand();
    
    if (command == "test_emergency") {
        // Switch to emergency state
        AppStateMachine& appTasks = getAppTasks();
        appTasks.switchToEmergency();
        // appTasks.switchToEmergency();
    } else if (command == "test_normal") {
        // Switch to normal state
        AppStateMachine& appTasks = getAppTasks();
        appTasks.switchToNormal();
    } else {
        // Invalid command
        spdlog::error("Invalid test command: {}", command);
    }

}