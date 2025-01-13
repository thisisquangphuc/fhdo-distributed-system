#include "tcp_utils.h"
#include <arpa/inet.h>  // For inet_addr
#include <cstring>      // For memset
#include <iostream>     // For logging
#include <unistd.h>     // For close()

int init_tcp(int port, std::string &error_message) {
    // Create a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        error_message = "Failed to create socket.";
        return -1;
    }

    // Allow address reuse
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error_message = "Failed to set socket options.";
        close(serverSocket);
        return -1;
    }

    // Define the server address
    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the specified port
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error_message = "Failed to bind socket to port " + std::to_string(port) + ".";
        close(serverSocket);
        return -1;
    }

    // Start listening on the socket
    if (listen(serverSocket, 5) < 0) {
        error_message = "Failed to start listening on socket.";
        close(serverSocket);
        return -1;
    }

    // Success
    error_message.clear();
    std::cout << "Server initialized on port " << port << "." << std::endl;
    return serverSocket;
}