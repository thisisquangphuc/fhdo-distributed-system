// C++ program to illustrate the client application in the
// socket programming
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY;

    // sending connection request
    int res = connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));

    if (clientSocket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    if (res < 0) {
        perror("Connection failed");
        return -1;
    }

    // sending data
    // std::string message = "Hello, server! " + std::string("expectedToken123");
    const char *authToken = "expectedToken123";
    ssize_t bytesSent = send(clientSocket, authToken, strlen(authToken), 0);
    if (bytesSent < 0) {
        perror("Send failed");
    }
    // closing socket
    close(clientSocket);

    return 0;
}