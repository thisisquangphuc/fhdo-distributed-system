#ifndef PLATOON_SERVER_H
#define PLATOON_SERVER_H

#include <string>
#include <map>
#include <netinet/in.h> // For sockaddr_in
#include <thread>
#include <uuid/uuid.h>

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

        static void sendResponse(int clientSocket, nlohmann::json response) {
            TruckMessage msg;
            std::string payload = msg.buildPayload(response);
            send(clientSocket, payload.c_str(), payload.size(), 0);
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

#endif // PLATOON_SERVER_H