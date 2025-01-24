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
        PlatoonServer::sendResponse(cli_socket, data);

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

        TruckMessage msg;
        msg.setCommand("slow_down");
        string payload = msg.buildPayload(data);
        for (const auto& truck : allTrucks) {
            if (truck.first == id) {
                // skip the leaving truck
                continue;
            }
            cli_socket = truck.second;
            PlatoonServer::sendResponse(cli_socket, payload);
        }

        //Response to leaving truck
        msg.setCommand("leave_start");
        payload = msg.buildPayload(data);
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
    //
    
}
void TruckEventFSM::handleEmergency(const TruckMessage& msg){
    // 
}

bool TruckEventFSM::Auth(const TruckMessage& msg, string truckID) {
    // bool Auth(const TruckMessage& msg, string truckID);
    return true;
}
