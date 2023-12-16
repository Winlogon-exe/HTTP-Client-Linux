#include"HttpClient.h"

int HttpClient::createSocket() {

    int sock;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
        throw std::runtime_error("Ошибка getaddrinfo");
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) {
        freeaddrinfo(res);
        throw std::runtime_error("Не удалось создать сокет");
    }

    connectSocket(sock, res);
    freeaddrinfo(res);
    return sock;
}

void HttpClient::connectSocket(int& sock, struct addrinfo* res) {
    if (connect(sock, res->ai_addr, res->ai_addrlen) == -1) {
        close(sock);
        throw std::runtime_error("Не удалось подключиться");
    }
}

std::vector<char> HttpClient::sendRequest(const std::string& requestType) {
    int sock = createSocket();

    std::string request = requestType + " / HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";
    send(sock, request.c_str(), request.size(), 0);

    char buffer[4096];
    int bytes_read;
    while ((bytes_read = read(sock, buffer, sizeof(buffer))) > 0) {
        data.insert(data.end(), buffer, buffer + bytes_read);
    }

    close(sock);
    return data;
}

void HttpClient::printData() const {
    for (char c : data) {
        std::cout << c;
    }
}

void HttpClient::clearData() {
    data.clear();
}