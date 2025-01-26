/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 14 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include "app.h"
AppStateMachine appTasks;

AppStateMachine& getAppTasks() {
    return appTasks;
}

void system_init() {
    init_logger();
    tcp_init();
    udp_init();
}

void initPlatoonData() {
    PlatoonDataManager& platoon_data = PlatoonDataManager::getInstance();
    platoon_data.init();
}

void AppStateMachine::init() {

    // Logic for initialization
    spdlog::info("Truck is in INIT state.");

    // Update platoon data
    initPlatoonData();

    currentState = State::IDLE;
    spdlog::info("Transiting from INIT to IDLE");
}

void AppStateMachine::idle() {
    // Logic for idle state
    // cout << "Truck is in IDLE state." << endl;
}

void AppStateMachine::normalOperation() {
    // Logic for normal operation
    // Just process message from the queue
}

void AppStateMachine::emergencyBrake() {
    /* Logic for emergency brake */ 
    // Broad cast UDP message

    nlohmann::json eMsg;
    eMsg["cmd"] = "emergency";
    eMsg["truck_id"] = "LEADING_001";
    eMsg["contents"] = json::object();
    eMsg["contents"]["brake_force"] = 0.8;
    eMsg["contents"]["speed"] = 5.0;

    // Send the broadcast
    // auto& udp = UDPBroadcastServer::getInstance();
    // udp.sendBroadcast(to_string(eMsg));
    auto& broadcastServer = UDPBroadcastServer::getInstance();
    // broadcastServer.initialize(59059);
    // broadcastServer.sendBroadcast("Emergency: Obstacle detected ahead!");
    broadcastServer.sendBroadcast(to_string(eMsg));

    //delay
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    spdlog::warn("Broadcasting EMERGENCY on UDP...");

}

void AppStateMachine::connectionLost() {
    // Logic for connection lost
}

void AppStateMachine::handleInvalidTasks() {
    // Logic for reconnecting
}