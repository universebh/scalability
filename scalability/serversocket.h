//
// Created by 陆清 on 3/24/20.
//

#ifndef H4D1_SERVERSOCKET_H
#define H4D1_SERVERSOCKET_H
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "GeneralException.h"
#include "utils.h"

/*struct client_obj{
    int delay_count;
    int bucket_item;
    int socket_fd;
    int thread_id;
};*/

// struct server_bucket{
//     int bucket_size=0;
//     int client_fd;//client
//     std::vector<double> bucket;
//     int delay_count;
//     int bucket_item;
//     int socket_fd;//server
//     int thread_id;
// };

class ServerSocket {
public:
    int socket_fd;  // listen on sock_fd, new connection on new_fd
    const char *hostname;
    const char *port;
    ServerSocket(): socket_fd(0), hostname(NULL),
                    port(NULL) {}
    ~ServerSocket() {
        // if (socket_fd == 0) {
        //     close(socket_fd);
        // }
        closeSockfd(socket_fd);
        // if (host_info_list != NULL) {
        //     free(host_info_list);
        // }
        // freeSockAddrList(host_info_list);
    }

    void setup();
    int ServerAccept();
    void closeSocket();

};

void ServerSocket::closeSocket() {
    closeSockfd(socket_fd);
}


void ServerSocket::setup() {
    struct addrinfo host_info;
    struct addrinfo * host_info_list = NULL;

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(hostname, port, &(host_info), &(host_info_list));
    if (status != 0) {
        throw GeneralException("cannot getaddrinfo");
    }

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        freeSockAddrList(host_info_list);
        throw GeneralException("cannot build socket()");
    }
    int yes = 1;
    status=setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if(status!=0){
        freeSockAddrList(host_info_list);
        throw GeneralException("ServerSocket: setsockopt");
    }
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    freeSockAddrList(host_info_list);

    if (status!=0) {
        closeSocket();
        throw GeneralException("bind failure");
    }
    status = listen(socket_fd, 50);//TODO: how many 50?
    if (status !=0) {
        throw GeneralException("listen failure");
    }
}

int ServerSocket::ServerAccept() {
    int client_connection_fd;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    client_connection_fd =
            accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
        perror("accept ");
        throw GeneralException("accept failure");
    }
    return client_connection_fd;
}

#endif //H4D1_SERVERSOCKET_H