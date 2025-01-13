#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include <netinet/in.h> // For sockaddr_in
#include <string>

int init_tcp(int port, std::string &error_message);

#endif // TCP_UTILS_H