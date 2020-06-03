#ifndef __CLIENTSOCKET_H__
#define __CLIENTSOCKET_H__
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include "GeneralException.h"
#include "utils.h"

#define BUFFER_SIZE 1024

class ClientSocket {
public:
    int socket_fd;  // listen on sock_fd, new connection on new_fd
    const char *hostname;
    const char *port;
    
public:
    // constructor
    ClientSocket(): socket_fd(-1), hostname(NULL), port(NULL) {}
    ClientSocket(const char * hostname, const char * port): socket_fd(-1), \
    hostname(hostname), port(port) {}
    // destructor
    ~ClientSocket() {
        // free linked list that stores
        // if (host_info_list != NULL) {
        //     freeaddrinfo(host_info_list);
        // }
        // freeSockAddrList(host_info_list);
        // if error occurs
        // if (socket_fd == -1) {
        //     close(socket_fd);
        //     socket_fd = -1;
        // }
        closeSockfd(socket_fd);
    }
    // setup as client
    void setUp();
    // send request
    void sendRequest(std::string msg);
    // receive response
    std::string receiveResponse();
};



/*
 * create and connect the socket in client side for communication with server side
 */
void ClientSocket::setUp(){
    // get address info
    struct addrinfo host_info;
    struct addrinfo * host_info_list = NULL;
    int status;
    struct addrinfo * p = NULL;
    memset(&host_info, 0, sizeof(host_info)); // zero out host info
    host_info.ai_family = AF_UNSPEC; // both IPv4 and IPv6
    host_info.ai_socktype = SOCK_STREAM; // TCP socket
    host_info.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(hostname, port, &host_info, &host_info_list)) != 0) {
        throw GeneralException("cannot getaddrinfo in client setup");
    }

    // loop through all the IPs in the linked list and connect to the first available one
    for (p = host_info_list; p != NULL; p = p->ai_next) {
        // create socket failed
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::perror("client side create socket failed");
            continue;
        }
        // connect failed
        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            closeSockfd(socket_fd);
            std::perror("client side connection failed");
            continue;
        }
        // create and connect socket both succeed or no address found
        break;
    }
    freeSockAddrList(host_info_list);
    // check if no address found
    if (p == NULL) {
        closeSockfd(socket_fd);
        throw GeneralException("found no IP address in the linked list");
    }
}


/*
 * send request to server side
 */
void ClientSocket::sendRequest(std::string msg) {
    // send it to the server
    if (send(socket_fd, msg.c_str(), msg.size(), 0) == -1) {
        throw GeneralException("failed to send message in client side");
    }
}


/*
 * handle the response received from the server side
 */
std::string ClientSocket::receiveResponse() {
    std::string res = "";
    char recvBuf[1000];
    // recv failed
    if (recv(socket_fd, recvBuf, BUFFER_SIZE, 0) == -1) {
        throw GeneralException("failed to receive message from server side");
    }
    // recv succeeds
    res = recvBuf;
    return res;
}
#endif