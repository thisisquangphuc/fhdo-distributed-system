#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include "communication/comm_msg.h"

std::mutex printMutex; // Mutex to prevent message interleaving when printing

void handleServerResponse(int clientSocket) {
    char buffer[1024];
    while (true) {
        // Receive data from the server
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived < 0) {
            perror("Receive failed");
            break;
        } else if (bytesReceived == 0) {
            // Server closed the connection
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Server disconnected." << std::endl;
            break;
        }

        // Null-terminate the received data and print it
        buffer[bytesReceived] = '\0';
        {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Received: " << buffer << std::endl;
        }
    }

    // Close the socket once the loop ends
    close(clientSocket);
}

int main() {
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Specify server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send connection request
    int res = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (res < 0) {
        perror("Connection failed");
        close(clientSocket);
        return -1;
    }

    // Send authentication token
    const char* authToken = "PHUC01QUYEN02ISSAC03DAO04";
    // Build a json in msg {
    //     "cmd":"auth",
    //     "contents": {
    //         "auth_key":"PHUC01QUYEN02ISSAC03DAO04"
    //     },
    //     "msg_id": "<uuid>",
    //     "timestamp":52451245
    // }
    char msg[1024];
    snprintf(msg, sizeof(msg), "{\"cmd\":\"auth\",\"contents\":{\"auth_key\":\"%s\"},\"msg_id\":\"%s\",\"timestamp\":%s}", authToken, TruckMessage::generateUUID().c_str(), std::to_string(TruckMessage::generateTimestamp()).c_str());


    ssize_t bytesSent = send(clientSocket, msg, strlen(msg), 0);
    if (bytesSent < 0) {
        perror("Send failed");
        close(clientSocket);
        return -1;
    }

    // Create a thread to handle server responses
    std::thread responseThread(handleServerResponse, clientSocket);

    // Wait for the response thread to finish
    responseThread.join();

    return 0;
}