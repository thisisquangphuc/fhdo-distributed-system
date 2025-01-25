#ifndef PLATOON_CLIENT_H
#define PLATOON_CLIENT_H

#include <string>
#include <netinet/in.h> // For sockaddr_in

class PlatoonClient {
public:
    PlatoonClient();
    ~PlatoonClient();

    bool startClient(int port, std::string host_ip, std::string &error_message);
    bool initUDPConnection(int port, std::string host_ip, std::string &error_message);
    void closeClientSocket();
    void setAuthToken(std::string key);
    bool sendMessage(std::string mess, std::string &error_message);
    std::string receiveMessage();
    std::string receiveUDPMessage();

private:
    int clientSocket;
    int UDPSocket;
    int port;
    int socketFd;
    bool isListening;
    struct sockaddr_in serverAddress;

    bool initSocketConnection(int port, std::string host_ip, std::string &error_message);
};

#endif // PLATOON_CLIENT_H
