#include "platoon_client.h"
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include "utils/logger.h"

// Constructor
PlatoonClient::PlatoonClient() : tcpSocket(-1), udpSocket(-1) {}

// Destructor
PlatoonClient::~PlatoonClient() {
    if (tcpSocket >= 0) {
        close(tcpSocket);
    }
    if (udpSocket >= 0) {
        close(udpSocket);
    }
}

// Initialize the socket connection with server
bool PlatoonClient::initTCPConnection(int port, std::string host_ip, std::string &error_message) {
    // creating socket
    tcpSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(port);
    tcpServerAddr.sin_addr.s_addr = inet_addr(host_ip.c_str());//INADDR_ANY;

    // sending connection request
    int res = connect(tcpSocket, (struct sockaddr*)&tcpServerAddr, sizeof(tcpServerAddr));

    if (tcpSocket < 0) {
        error_message = "Failed to create TCP socket to port " + std::to_string(port);
        close(tcpSocket);
        return false;
    }

    if (res < 0) {
        error_message = "Failed to connect to server on socket port " + std::to_string(port);
        close(tcpSocket);
        return false;
    }

    error_message.clear();
    return true;
}

// Initialize the UDP connection with server
bool PlatoonClient::initUDPConnection(int port, std::string host_ip, std::string &error_message) {
//    const char *hello = "Hello from following truck.";

    // creating socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        error_message = "Failed to create UDP socket to port " + std::to_string(port);
        return false;
    }
    memset(&udpServerAddr, 0, sizeof(udpServerAddr));

    // specifying address
    udpServerAddr.sin_family = AF_INET;
    udpServerAddr.sin_port = htons(port);
    udpServerAddr.sin_addr.s_addr = inet_addr(host_ip.c_str());//INADDR_ANY;
    
//    sendto(udpSocket, (const char *)hello, strlen(hello), 
//        MSG_CONFIRM, (const struct sockaddr *) &udpServerAddr, sizeof(udpServerAddr)); 

    error_message.clear();
    return true;
}

// Start connecting to the server
bool PlatoonClient::startClient(int tcp_port, int udp_port, std::string host_ip, std::string &error_message) {
    std::string error_mess;
    bool tcpConnection = initTCPConnection(tcp_port, host_ip, error_message);
    bool udpConnection = initUDPConnection(udp_port, host_ip, error_mess);
    error_message = error_message + "\n" + error_mess;
    return (tcpConnection && udpConnection);
}

// Close socket connection with server
void PlatoonClient::closeClientSocket() {
    close(tcpSocket);
    close(udpSocket);
}

// 
bool PlatoonClient::sendMessage(std::string mess, std::string &error_message) {
    ssize_t bytesSent = send(tcpSocket, mess.c_str(), mess.length(), 0);
    if (bytesSent < 0) {
        error_message = "Failed to send message: " + mess;
        return false;
    }
    return true;
}

// 
std::string PlatoonClient::receiveMessage() {
    char buffer[1024] = {0};

    while (true) {
        int bytesReceived = recv(tcpSocket, buffer, sizeof(buffer), 0);
        printf("[%s]-%d %s\n", __func__, bytesReceived, buffer);

//        if (bytesReceived <= 0) {
//          std::cout << "Connection is closed.";
//          break;
//        }

        std::string receivedMessage(buffer, bytesReceived);
        return receivedMessage;
    }
//    return NULL;
}

//
std::string PlatoonClient::receiveUDPMessage() {
    int bytesReceived;
    socklen_t len;
    char buffer[1024] = {0};

    bytesReceived = recvfrom(udpSocket, (char*)buffer, 1024, MSG_WAITALL, (struct sockaddr *) &udpServerAddr, &len);;
    buffer[bytesReceived] = '\0';

    // printf("[%s]-%d %s\n", __func__, bytesReceived, buffer);
    std::string receivedMessage(buffer);
    spdlog::warn("Received UDP message: {}", receivedMessage);
    return receivedMessage;
}
