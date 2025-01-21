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

using namespace std;

typedef enum {
    INIT,
    IDLE,
    NORMAL_OPERATION,
    SPEED_UP,
    SLOW_DOWN,
    EMERGENCY_BRAKE,
    CONNECTION_LOST
} TruckState;

class AppStateMachine {
    private:
        TruckState currentState;
        vector<string> pre_shared_keys;

    public:
        // enum class State { INIT, IDLE, NORMAL_OPERATION, SPEED_UP, SLOW_DOWN, EMERGENCY_BRAKE, CONNECTION_LOST };
        AppStateMachine();

        void init();
        void idle();
        void normalOperation();
        void speedUp();
        void slowDown();
        void emergencyBrake();
        void connectionLost();

        TruckState getCurrentState() const;

        void handleEvent(const TruckMessage& msg) {
            cout << "Handle event: " << endl;

            while (true)
            { 
                switch (getCurrentState()) {
                    case INIT:
                        init();
                        break;
                    case IDLE:
                        idle();
                        break;
                    case NORMAL_OPERATION:
                        normalOperation();
                        break;
                    case SPEED_UP:
                        speedUp();
                        break;
                    case SLOW_DOWN:
                        slowDown();
                        break;
                    case EMERGENCY_BRAKE:
                        emergencyBrake();
                        break;
                    case CONNECTION_LOST:
                        connectionLost();
                        break;
                }
                //sleep for 1 second
                std::this_thread::sleep_for(std::chrono::seconds(1));
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
    // PlatoonServer server(port);
    // server.setPort(port);
    // server.setIP(host_ip);
    // string error_message;
    // if (!server.startServer(error_message)) {
    //     std::cerr << "Error starting server: " << error_message << std::endl;
    // }
    // std::thread serverThread(&PlatoonServer::start, &server);

    //Create thread
    std::thread socket_server(start_platoon_server, port, host_ip);
    socket_server.detach();
    // serverThread.detach();

    return 0;
}

void system_init();

#endif
