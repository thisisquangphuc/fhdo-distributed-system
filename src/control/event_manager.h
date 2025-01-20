/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include "utils/logger.h"
#include "communication/comm_msg.h"
#include "utils/env.h"

class TruckStateMachine {
public:
    enum class State { Idle, Authen, Join, Leave, Communicate, Emergency, Obstacle };
    std::mutex databaseMutex;
    std::string generateTruckID() {
        // Generate a unique Truck ID
        uuid_t uuid;
        char uuidStr[37];
        uuid_generate(uuid);
        uuid_unparse(uuid, uuidStr);
        return std::string("TR00") + std::string(uuidStr);
    }

    TruckStateMachine() : currentState(State::Idle) {}

    void handleEvent(const TruckMessage& msg) {
        switch (currentState) {
        case State::Idle:
            handleIdle(msg);
            break;
        case State::Authen:
            handleAuthen(msg);
            break;
        case State::Join:
            handleJoin(msg);
            break;
        case State::Leave:
            handleLeave(msg);
            break;
        case State::Communicate:
            handleCommunicate(msg);
            break;
        case State::Emergency:
            handleEmergency(msg);
            break;
        default:
            spdlog::warn("Unhandled state");
        }
    }

    State getState() const { return currentState; }

private:
    State currentState;

    void handleIdle(const TruckMessage& msg);
    void handleJoin(const TruckMessage& msg);
    void handleLeave(const TruckMessage& msg);
    void handleCommunicate(const TruckMessage& msg);
    void handleEmergency(const TruckMessage& msg);
    void handleAuthen(const TruckMessage& msg);
};

