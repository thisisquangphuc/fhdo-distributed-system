/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "utils/logger.h"
#include "communication/platoon_server.h"

using namespace std;

int main() {

    //init logger
    init_logger();


    spdlog::info("TEST - Starting the Truck Platooning system...");
    spdlog::debug("TEST - Debugging message: System initialized correctly.");
    spdlog::warn("TEST - Warning: Low fuel detected.");
    spdlog::error("TEST - Error: Connection to the leading truck failed.");

    int port = 8080;
    std::string error_message;

    PlatoonServer platoonServer(port);
    if (!platoonServer.startServer(error_message)) {
        std::cerr << "Error starting server: " << error_message << std::endl;
        return 1;
    }

    std::cout << "Server started on port " << port << ". Accepting connections..." << std::endl;
    platoonServer.acceptConnections();
    
    return 0;
}