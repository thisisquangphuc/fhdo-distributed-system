#include "platoon_server.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

// Constructor
PlatoonServer::PlatoonServer(int port) : serverSocket(-1), port(port) {}

// Destructor
PlatoonServer::~PlatoonServer() {
    if (serverSocket >= 0) {
        close(serverSocket);
    }
}

// Initialize the server socket
bool PlatoonServer::initializeSocket(std::string &error_message) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        error_message = "Failed to create socket.";
        return false;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error_message = "Failed to set socket options.";
        close(serverSocket);
        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error_message = "Failed to bind socket to port " + std::to_string(port);
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 10) < 0) {
        error_message = "Failed to listen on socket.";
        close(serverSocket);
        return false;
    }

    error_message.clear();
    return true;
}

// Start the server
bool PlatoonServer::startServer(std::string &error_message) {
    return initializeSocket(error_message);
}

// Accept connections
void PlatoonServer::acceptConnections() {
    std::cout << "Waiting for trucks to join the platoon..." << std::endl;

    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept connection." << std::endl;
            continue;
        }

        std::cout << "Truck connected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;

        // Authenticate the truck
        if (!authenticateTruck(clientSocket, "expectedToken123")) {
            std::cerr << "Authentication failed. Closing connection." << std::endl;
            close(clientSocket);
            continue;
        }

        // Save the truck connection
        truckConnections[clientSocket] = clientAddress;
        std::cout << "Truck authenticated and added to the platoon." << std::endl;
    }
}

// Authenticate a truck
bool PlatoonServer::authenticateTruck(int clientSocket, const std::string &expectedToken) {
    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    std::string receivedToken(buffer);
    return receivedToken == expectedToken;
}