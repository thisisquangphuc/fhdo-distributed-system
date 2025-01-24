/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 14 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#ifndef APP_H
#define APP_H

#include <string>
#include <map>
#include <netinet/in.h> // For sockaddr_in
#include "communication/platoon_server.h"

#include "utils/env.h"
#include "utils/logger.h"
#include "communication/comm_msg.h"
#include "utils/keygen.h"
#include "control/monitor.h"

using namespace std;

class AppStateMachine {
    private:
        enum class State { INIT, IDLE, NORMAL_OPERATION, EMERGENCY, CONNECTION_LOST, INVALID_TASKS };
        State currentState;
        vector<std::string> pre_shared_keys;

    public:
        AppStateMachine() : currentState(State::INIT) {}

        void init();
        void idle();
        void normalOperation();
        void emergencyBrake();
        void connectionLost();
        void handleInvalidTasks();  

        State getCurrentState() { return currentState; }

        void handleEvent() {
            while (true)
            { 
                switch (getCurrentState()) {
                    case State::INIT:
                        init();
                        break;
                    case State::IDLE:
                        idle();
                        break;
                    case State::NORMAL_OPERATION:
                        normalOperation();
                        break;
                    case State::EMERGENCY:
                        emergencyBrake();
                        break;
                    case State::CONNECTION_LOST:
                        connectionLost();
                        break;
                    case State::INVALID_TASKS:
                        handleInvalidTasks();
                        break;
                    default:
                        break;
                }
                //sleep for 100 milliseconds
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
};

void inline start_platoon_server(int port, std::string host_ip) {
    
    PlatoonServer platoonServer(port);
    std::string error_message;
    platoonServer.setIP(host_ip);
    if (!platoonServer.startServer(error_message)) {
        std::cerr << "Error starting server: " << error_message << std::endl;
    }

    std::cout << "Server started on port " << port << ". Accepting connections..." << std::endl;
    // platoonServer.acceptConnections();
    platoonServer.start();
}

inline int tcp_init() {
    int port = env_get_int("PORT", 8080);
    string host_ip = env_get("HOST_IP", "127.0.0.1");

    // Print port and host info 
    spdlog::info("Port: {}", port);
    spdlog::info("Host IP: {}", host_ip);

    int num_trucks = env_get_int("NUM_TRUCKS", TOTAL_TRUCKS);

    std::cout << "Starting server..." << std::endl;

    //Create thread
    std::thread socket_server(start_platoon_server, port, host_ip);
    socket_server.detach();

    return 0;
}

void system_init();

#endif
