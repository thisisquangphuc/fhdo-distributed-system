#ifndef PLATOON_SERVER_H
#define PLATOON_SERVER_H

#include <string>
#include <map>
#include <netinet/in.h> // For sockaddr_in
#include <thread>
#include <uuid/uuid.h>
#include <unistd.h>

#include "utils/env.h"
#include "utils/config.h"
#include "communication/comm_msg.h"
#include "utils/logger.h"

using namespace std;
class PlatoonServer {
    private:
        int serverSocket;
        bool isRunning;
        int port;
        std::string host_ip;
        std::map<int, sockaddr_in> truckConnections; // Maps client socket to truck info

        bool initializeSocket(std::string &error_message);
        void handleTruckSocket(int clientSocket);

    public:
        PlatoonServer(int port);

        ~PlatoonServer();

        void setPort(int port) { this->port = port; }
        void setIP(std::string host_ip) { this->host_ip = host_ip; }
        bool startServer(std::string &error_message);
        void acceptConnections();
        bool authenticateTruck(std::string key, const std::string &expectedToken);

        void start();
        void stop();

        static void sendResponse(int clientSocket, const std::string& response) {
            // TruckMessage msg;
            // std::string payload = msg.buildPayload(response);
            // std::string payload = msg.buildPayload(response);
            send(clientSocket, response.c_str(), response.size(), 0);
        }

        std::string generateTruckID() {
            // Generate a unique Truck ID
            uuid_t uuid;
            char uuidStr[37];
            uuid_generate(uuid);
            uuid_unparse(uuid, uuidStr);
            return std::string("TR00") + std::string(uuidStr);
        }

};

class UDPBroadcastServer {
    private:
        int broadcastSocket;
        struct sockaddr_in broadcastAddress;
        std::mutex broadcastMutex;

        // Private constructor for Singleton
        UDPBroadcastServer() : broadcastSocket(-1) {}

        // Delete copy constructor and assignment operator
        UDPBroadcastServer(const UDPBroadcastServer&) = delete;
        UDPBroadcastServer& operator=(const UDPBroadcastServer&) = delete;

    public:
        ~UDPBroadcastServer() {
            if (broadcastSocket != -1) {
                close(broadcastSocket);
            }
        }

        static UDPBroadcastServer& getInstance() {
            static UDPBroadcastServer instance;
            return instance;
        }

        void initialize(uint16_t port);

        void sendBroadcast(const std::string& message);
};
#endif // PLATOON_SERVER_H