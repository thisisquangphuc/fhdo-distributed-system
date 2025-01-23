/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 14 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include "app.h"

void system_init() {
    init_logger();
    tcp_init();
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
}

void AppStateMachine::emergencyBrake() {
    // Logic for emergency brake
}

void AppStateMachine::connectionLost() {
    // Logic for connection lost
}

void AppStateMachine::handleInvalidTasks() {
    // Logic for reconnecting
}