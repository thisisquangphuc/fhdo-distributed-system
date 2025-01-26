#include "platoon_server.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include "control/trucks_manager.h"
#include "control/event_manager.h"

// Constructor
PlatoonServer::PlatoonServer(int port) : serverSocket(-1), port(port) {}

// Destructor
PlatoonServer::~PlatoonServer() {
    if (serverSocket >= 0) {
        close(serverSocket);
    }
}

void PlatoonServer::handleTruckSocket(int clientSocket) {
    char buffer[1024] = {0};
    std::string rawMessage;
    std::string body;
    bool isAuthenticated = false;
    string truck_id = "";
    while (true) {
        // Clear rawMessage to handle subsequent messages
        rawMessage.clear();
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Connection closed by truck.\n";
            break;
        }

        // std::string message(buffer, bytesReceived);
        // Append received data to the raw message
        rawMessage.append(buffer, bytesReceived);

        // Find the end of the headers (blank line: \r\n\r\n)
        size_t headerEnd = rawMessage.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            // Extract the body (skip the headers)
            body = rawMessage.substr(headerEnd + 4); // +4 to skip \r\n\r\n

            // Process the body
            std::cout << "Received body: " << body << std::endl;
        } else {
            body = rawMessage;
        }

        std::cout << "Message from truck: " << body << std::endl;
        try
        {
            TruckMessage truckMessage(body);
            // Get authen key 
            if (!isAuthenticated && !truckMessage.isTesting()) {
                if (truckMessage.getAuthenKey() == "") {
                    std::cerr << "Truck did not send authen key." << std::endl;
                    continue;
                }
                spdlog::info("Authen key: {}", truckMessage.getAuthenKey());
                string session_key = env_get("SESSION_KEY", DEFAULT_SESSION_KEY);

                if (authenticateTruck(truckMessage.getAuthenKey(), session_key)) {
                    spdlog::info("Authentication successful.");
                    isAuthenticated = true;

                    // Generate truck ID
                    truck_id = generateTruckID();
                    // Add truck id to list
                    TruckManager& truckManager = TruckManager::getInstance();
                    truckManager.addTruck(truck_id, clientSocket);

                    truckMessage.setTruckID("LEADING_001");
                    truckMessage.setCommand("auth_ok");
                    json contents = {{"id", truck_id}};

                    string response = truckMessage.buildPayload(contents);
                    send(clientSocket, response.c_str(), response.size(), 0);

                    continue;

                } else {
                    spdlog::error("Authentication failed.");
                    // send error
                    std::string rsp = truckMessage.buildPayload({{"error_code", "auth_failed"}});
                    send(clientSocket, rsp.c_str(), rsp.size(), 0);
                    continue;
                } 
            }
            // Handle message
            spdlog::info("Truck message: {}", truckMessage.serialize());
            string cmd = truckMessage.getCommand();
            if (cmd == "emergency" || cmd == "obstacle") {
                //high priority
                emergencyQueue.enqueueCommand(body);
            } else {
                //low priority
                regularQueue.enqueueCommand(body);
            }
            // std::string response = truckMessage.serialize();
            // send(clientSocket, response.c_str(), response.size(), 0);
        }
        catch (const nlohmann::json::exception& e)
        {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            std::string response = R"({"error_code": "message_format_error"})";
            send(clientSocket, response.c_str(), response.size(), 0);
            continue;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error handling truck message: " << e.what() << std::endl;
            std::string response = R"({"error_code": "message_format_error"})";
            send(clientSocket, response.c_str(), response.size(), 0);
            continue;
        } catch(const std::exception& e){
            std::cerr << "Error handling truck message: " << e.what() << std::endl;
            std::string response = R"({"error_code": "message_parse_error"})";
            send(clientSocket, response.c_str(), response.size(), 0);
            continue;
        } 

    }
    close(clientSocket);
    std::cout << "Finished handling truck.\n";
    TruckManager& truckManager = TruckManager::getInstance();
    truckManager.removeTruck(truck_id);
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
    serverAddress.sin_addr.s_addr = inet_addr(host_ip.c_str());//INADDR_ANY;

    if (::bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
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
            // std::cerr << "Failed to accept connection." << std::endl;
            continue;
        }

        std::cout << "Truck connected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;

        // Authenticate the truck
        // if (!authenticateTruck(clientSocket, "expectedToken123")) {
        //     std::cerr << "Authentication failed. Closing connection." << std::endl;
        //     close(clientSocket);
        //     continue;
        // }

        // Save the truck connection
        truckConnections[clientSocket] = clientAddress;
        std::cout << "Truck authenticated and added to the platoon." << std::endl;

    }
}

// Authenticate a truck
bool PlatoonServer::authenticateTruck(string key, const std::string &expectedToken) {
    // char buffer[1024] = {0};
    // recv(clientSocket, buffer, sizeof(buffer), 0);

    std::string receivedToken(key);
    return receivedToken == expectedToken;
}

void PlatoonServer::start() {
    isRunning = true;

    while (isRunning) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            if (!isRunning) break;
            std::cerr << "Error accepting connection.\n";
            continue;
        }

        std::cout << "New truck connected.\n";

        // Create a new thread to handle the client
        
        std::thread clientThread(&PlatoonServer::handleTruckSocket, this, clientSocket);
        clientThread.detach();
    }
}

void PlatoonServer::stop() {
    isRunning = false;
    close(serverSocket);
    std::cout << "Server stopped.\n";
}


/* UDP */
void UDPBroadcastServer::initialize(uint16_t port) {
    std::lock_guard<std::mutex> lock(broadcastMutex);

    if (broadcastSocket != -1) {
        throw std::runtime_error("UDPBroadcastServer is already initialized.");
    }

    broadcastSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (broadcastSocket < 0) {
        throw std::runtime_error("Failed to create UDP socket.");
    }

    int broadcastEnable = 1;
    if (setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        close(broadcastSocket);
        throw std::runtime_error("Failed to enable broadcast option.");
    }

    broadcastAddress.sin_family = AF_INET;
    broadcastAddress.sin_port = htons(port);
    broadcastAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_BROADCAST; // Broadcast address
}

std::string PlatoonServer::generateTruckID() {
    // Generate a unique Truck ID
    uuid_t uuid;
    char uuidStr[37];
    uuid_generate(uuid);
    uuid_unparse(uuid, uuidStr);
    return std::string("TR00") + std::string(uuidStr);
}

void UDPBroadcastServer::sendBroadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(broadcastMutex);

    if (broadcastSocket == -1) {
        throw std::runtime_error("UDPBroadcastServer is not initialized.");
    }

    ssize_t sentBytes = sendto(broadcastSocket, message.c_str(), message.size(), 0,
                                (struct sockaddr*)&broadcastAddress, sizeof(broadcastAddress));
    if (sentBytes < 0) {
        throw std::runtime_error("Failed to send broadcast message.");
    }
}   
