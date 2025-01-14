#ifndef PLATOON_SERVER_H
#define PLATOON_SERVER_H

#include <string>
#include <map>
#include <netinet/in.h> // For sockaddr_in

class PlatoonServer {
public:
    PlatoonServer(int port);
    ~PlatoonServer();

    bool startServer(std::string host_ip, std::string &error_message);
    void acceptConnections();
    bool authenticateTruck(int clientSocket, const std::string &expectedToken);

private:
    int serverSocket;
    int port;
    std::map<int, sockaddr_in> truckConnections; // Maps client socket to truck info

    bool initializeSocket(std::string host_ip, std::string &error_message);
};

#endif // PLATOON_SERVER_H