#ifndef PLATOON_SERVER_H
#define PLATOON_SERVER_H

#include <string>
#include <map>
#include <netinet/in.h> // For sockaddr_in
#include <thread>
#include "utils/env.h"
#include "utils/config.h"

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

};

#endif // PLATOON_SERVER_H