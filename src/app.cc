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

AppStateMachine::AppStateMachine() : currentState(TruckState::INIT) {}

void AppStateMachine::init() {

    // Logic for initialization
    // Generate pre-shared keys for number of trucks
    // store keys in a list
    int num_trucks = env_get_int("NUM_TRUCKS", TOTAL_TRUCKS);
    pre_shared_keys = generate_pre_shared_key(num_trucks);
    //print pre-shared keys to terminal with key map
    for (int i = 0; i < num_trucks; i++) {
        spdlog::info("Truck {} pre-shared key: {}", i, pre_shared_keys[i]);
    }
    cout << "Truck is in INIT state." << endl;
    // Set current state to IDLE
    // app_state = IDLE;
    currentState = TruckState::IDLE;

    spdlog::info("Transiting from INIT to IDLE");
}

void AppStateMachine::idle() {
    // Logic for idle state
    // cout << "Truck is in IDLE state." << endl;
}

void AppStateMachine::normalOperation() {
    // Logic for normal operation
}

void AppStateMachine::speedUp() {
    // Logic for speeding up
}

void AppStateMachine::slowDown() {
    // Logic for slowing down
}

void AppStateMachine::emergencyBrake() {
    // Logic for emergency brake
}

void AppStateMachine::connectionLost() {
    // Logic for connection lost
}

TruckState AppStateMachine::getCurrentState() const {
    return currentState;
}