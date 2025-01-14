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

using namespace std;

inline int tcp_init() {
    int port = env_get_int("PORT", 8080);
    string host_ip = env_get("HOST_IP", "127.0.0.1");

    // Print port and host info 
    spdlog::info("Port: {}", port);
    spdlog::info("Host IP: {}", host_ip);
    
    std::string error_message;

    PlatoonServer platoonServer(port);
    if (!platoonServer.startServer(host_ip, error_message)) {
        std::cerr << "Error starting server: " << error_message << std::endl;
        return 1;
    }

    std::cout << "Server started on port " << port << ". Accepting connections..." << std::endl;
    platoonServer.acceptConnections();
    return 0;
}

void system_init();

#endif
