#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

class HttpClient {
public:
    HttpClient(const std::string& hostname, int port)
    : hostname(hostname), port(port) {}

    std::vector<char> sendRequest(const std::string& requestType = "GET");
    void printData() const;
    void clearData();
private:
    std::vector<char> data;
    std::string hostname;
    int port;
    

    int createSocket();
    void connectSocket(int& sock, struct addrinfo* res);
};
     
#endif // HTTPCLIENT_H
