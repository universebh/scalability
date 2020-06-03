#ifndef __UTILS_H__
#define __UTILS_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <sys/time.h>

void closeSockfd(int & socket_fd) {
    if (socket_fd >= 0) {
        close(socket_fd);
        socket_fd = -1;
    }
}

void freeSockAddrList(addrinfo * host_info_list) {
    if (host_info_list != NULL) {
        freeaddrinfo(host_info_list);
        host_info_list = NULL;
    }
    // freeaddrinfo(host_info_list);
}

double delay(int req_delay) {
    struct timeval start, check;
    double elapsed_seconds;
    gettimeofday(&start, NULL);
    do {
        gettimeofday(&check, NULL);
        elapsed_seconds = (check.tv_sec + (check.tv_usec/1000000.0)) - \
        (start.tv_sec + (start.tv_usec/1000000.0));
    } while (elapsed_seconds < req_delay);
    // std::cout<<"elapsed_second "<<elapsed_seconds<<std::endl;
    return elapsed_seconds;
}

void printHints() {
    // print some hints
    // client: c num_requests delay variation num_buckets
    // server: s thread_form num_buckets
    std::cout << "Please open two terminals, one for the server side and the other for the client side." << std::endl;
    std::cout << "You should enter three parameters. The first one determines the side. Enter s to run server side codes,"
                 " enter c to run client side codes." << std::endl;
    std::cout << "For the client side, the second parameter is the number of requests." << std::endl;
    std::cout << "For the server side, as second parameter enter 1 for opening a thread pool or 0 to create "
                 "thread per request." << std::endl;
    std::cout << "For the client side, as third parameter enter the variation of delay, 0 for small (1-3s) and 1 for "
                 "large (1-20s)." << std::endl;
    std::cout << "For the server side, as third parameter enter the bucket size. 1 for 32 buckets, 2 for 128 buckets, "
                 "3 for 512 buckets, 4 for 2048 buckets." << std::endl;
    std::cout << "For the client side, as fourth parameter enter the server side bucket size. 1 for 32 buckets, 2 for 128"
                 "buckets, 3 for 512 buckets, 4 for 2048 buckets." << std::endl;    
}

#endif