#include "platoon_client.h"
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h> 
#include <unistd.h>
#include <arpa/inet.h>

// Constructor
PlatoonClient::PlatoonClient() : clientSocket(-1) {}

// Destructor
PlatoonClient::~PlatoonClient() {
    if (clientSocket >= 0) {
        close(clientSocket);
    }
}

// Initialize the socket connection with server
bool PlatoonClient::initSocketConnection(int port, std::string host_ip, std::string &error_message) {
    // creating socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(host_ip.c_str());//INADDR_ANY;

    // sending connection request
    int res = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (clientSocket < 0) {
        error_message = "Failed to create socket to port " + std::to_string(port);
        close(clientSocket);
        return false;
    }

    if (res < 0) {
        error_message = "Failed to connect to server on socket port " + std::to_string(port);
        close(clientSocket);
        return false;
    }

    error_message.clear();
    return true;
}

// Start connecting to the server
bool PlatoonClient::startClient(int port, std::string host_ip, std::string &error_message) {
    return initSocketConnection(port, host_ip, error_message);
}

//
void PlatoonClient::setAuthToken(std::string key) {
    authToken = key;
}

// Close socket connection with server
void PlatoonClient::closeClientSocket() {
    close(clientSocket);
}

// 
bool PlatoonClient::sendMessage(std::string mess, std::string &error_message) {
    ssize_t bytesSent = send(clientSocket, mess.c_str(), mess.length(), 0);
    if (bytesSent < 0) {
        error_message = "Failed to send message: " + mess;
        return false;
    }
    return true;
}

// 
std::string PlatoonClient::receiveMessage() {
    char buffer[1024] = {0};

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("[%s]-%d %s\n", __func__, bytesReceived, buffer);

    std::string receivedMessage(buffer);

    return (bytesReceived > 0) ? receivedMessage : NULL;
}

//
std::string PlatoonClient::readMessage() {
    char buffer[1024] = {0};

    read(clientSocket, buffer, 1024 - 1);
    printf("%s\n", buffer);
    
    std::string receivedMessage(buffer);
    return receivedMessage;
}
