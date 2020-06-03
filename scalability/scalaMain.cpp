#include <string>
#include <cstring>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <cstdio>
#include <cctype>
#include <iostream>
#include <string>
#include "serversocket.h"
#include "clientsocket.h"
#include "utils.h"
#include "threadpool/threadpool.h"

#define THREAD_SIZE 512
std::mutex mtx;

/*
 * clientSocketFD: file descriptor of socket on the client side
 * isLargeVariation: 0 for small delay variation(1-3s), 1 for large(1-20s)
 * count: number of total handled requests
 */
// struct handleCLientParameter {
//     bool isLargeVariation;
//     int count;
//     //int bucketSize;
//     const char * hostname;
//     const char * port;
// };


/*
 * func params: a struct that includes a boolean indication variation type
 * an integer for total number of requests
 * an integer for number of buckets
 */
void handleClient(bool isLargeVariation, const char * hostname, const char * port, int client_bucket_size) {

    // set up client socket
    ClientSocket clientSocket(hostname, port);
    try {
        clientSocket.setUp();
        // freeSockAddrList(clientSocket.host_info_list);
    }
    catch (GeneralException & e) {
        // freeSockAddrList(clientSocket.host_info_list);
        return;
    }

    // create a message in a range according to delay variation type
    int delayVariation, bucketIdx;
    // small variation
    if (!isLargeVariation) {
        delayVariation = rand() % 3 + 1;
    }
    // large variation
    else {
        delayVariation = rand() % 20 + 1;
    }
    // TODO
    // assume bucket size is 512
    bucketIdx = rand() % client_bucket_size;
    std::string sendMsg = std::to_string(delayVariation) + "," + std::to_string(bucketIdx) + "\n";
    // send request to server
    clientSocket.sendRequest(sendMsg);
    try {
        std::lock_guard<std::mutex> lck(mtx);
        std::cout << "client send msg: " << sendMsg;
    }
    catch(GeneralException & e) {
        // freeSockAddrList(clientSocket.host_info_list);
        throw GeneralException("lock guard failed.");
    } 
    // receive response from server
    std::string recvMsg = clientSocket.receiveResponse();
    // freeSockAddrList(clientSocket.host_info_list);
}

int prepareServer(ServerSocket & serverSocket) {    
    int client_fd = serverSocket.ServerAccept();

    if (client_fd < 0) {
        throw GeneralException("Accept failed in prepareServer");
    }

    return client_fd;
}

std::string serverRecv(int client_fd) {
    char buffer[1024];
    memset(buffer, 0, 1024);
    int numBytes = recv(client_fd, buffer, sizeof(buffer), 0);
    std::string recvMsg = "";
    if (numBytes > 0) {
        recvMsg = buffer;
        return recvMsg;
    }
    else if (numBytes == 0) {
        return recvMsg;
    }
    else {
        throw GeneralException("Server receive failed");
    }
}

void serverSend(int client_fd, std::string & sendMsg) {
    if (client_fd < 0) {
        throw GeneralException("Invalid client_fd in serverSend");
    }
    if (send(client_fd, sendMsg.c_str(), sendMsg.size(), 0) == -1) {
        throw GeneralException("Server send failed");
    }
}

/*
* compute the current throughput
*/
double throughputCnt(struct timeval& start, int count) {
    struct timeval current;
    double elapsed_seconds;
    gettimeofday(&current, NULL);
    elapsed_seconds = current.tv_sec + current.tv_usec/1000000.0 - (start.tv_sec + start.tv_usec/1000000.0);
    return count/elapsed_seconds;
}

void handleServer(int client_fd, int & count, std::vector<double> & bucket, struct timeval& start_time) {
    // recv string
    std::string recvMsg = serverRecv(client_fd);
    if (recvMsg.size() == 0) {
        // std::cerr << request_id << ": " << "server received nothing.\n";
        return;
    }
    
    // parse string
    std::string whole, first, last;
    std::string delimiter = "\n";
    whole = recvMsg.substr(0, recvMsg.find(delimiter));
    delimiter = ",";
    first = whole.substr(0, whole.find(delimiter));
    last = whole.substr(whole.find(delimiter) + 1);
    int bucketIdx = 0;
    double delayTm = 0;
    delayTm = std::stoi(first);
    bucketIdx = std::stoi(last);

    delay(delayTm);

    // update bucket and increase count
    try {
        std::lock_guard<std::mutex> lck(mtx);
        bucket[bucketIdx] += delayTm;
        ++count;
        // compute throughput
        double current_throughput = throughputCnt(start_time, count);
        // print modified buckets and current delay count
        std::cout << "Count: " << count << "\t" << \
        "Bucket " << bucketIdx << ": " << bucket[bucketIdx] << "\t" << "Current throughput: " << current_throughput \
        << std::endl;
    }
    catch(GeneralException & e) {
        throw GeneralException("lock guard failed.");
    } 

    // create message to client
    std::string sendMsg = "Bucket " + std::to_string(bucketIdx + 1) + ": " + std::to_string(bucket[bucketIdx]) + "\n";
    
    // send
    serverSend(client_fd, sendMsg);
}

// void handleServerThreadpool(int request_id, 
// std::queue<int> & requestQueue, int & count, std::vector<double> & bucket) {

// }

/*
 * func parameters: 3 parameters
 */
int main(int argc, char **argv) {
    srand((unsigned)time(NULL));
    // default parameters
    int bucket_size = 32, client_bucket_size = 32;
    bool isLargeVariation = false;

    // print user messages
    printHints();

    // invalid number of parameters
    if (argc != 4 && argc != 5) {
        std::cerr << "Invalid number of parameters, please make sure the number of parameters is correct." << std::endl;
        return EXIT_FAILURE;    
    }

    // invalid first parameter
    if (strlen(argv[1]) != 1) {
        std::cerr << "Invalid parameter [1], please enter c to run client side codes and s to run server side codes." <<
        std::endl;
        return EXIT_FAILURE;
    }
    char checkServerOrClient = tolower((int) *argv[1]);

    // 5 parameters, run client side codes
    if (argc == 5) {
        std::cout << "Client side number of requests is: " << argv[2] << std::endl;
        std::cout << "Client side number of buckets is: " << argv[4] << std::endl;
        // run client codes
        if (checkServerOrClient != 'c') {
            std::cerr << "Invalid parameter [1], please enter c to run client side codes." <<
            std::endl;
            return EXIT_FAILURE;
        }

        else {
            // parse terminal arguments
            int num_requests = std::atoi(argv[2]);
            if (num_requests == 0) {
                std::cerr << "Invalid parameter [2], please enter c to run client side codes." <<
                std::endl;
                return EXIT_FAILURE;                
            }
            std::string checkLargeVariation = argv[3];
            if (checkLargeVariation == "0" || checkLargeVariation == "1") {
                isLargeVariation = std::atoi(argv[3]) == 1 ? true : false;
            }
            else {
                std::cerr << "Invalid parameter [3], please enter 0 for small variation or 1 for large variation.\n";
                return EXIT_FAILURE;
            }

            int checkBucketSize = std::atoi(argv[4]);
            switch (checkBucketSize) {
                case 1:
                    client_bucket_size = 32;
                    break;
                case 2:
                    client_bucket_size = 128;
                    break;
                case 3:
                    client_bucket_size = 512;
                    break;
                case 4:
                    client_bucket_size = 2048;
                    break;
                default:
                    std::cerr << "Invalid parameter [4], please enter 1 for 32 buckets, 2 for 128 buckets, "
                    "3 for 512 buckets, 4 for 2048 buckets.";
                    return EXIT_FAILURE;
            }

            // client setup
            const char * hostname = "vcm-13661.vm.duke.edu";
            const char * port = "12345";
            for (int i = 0; i < num_requests; i++) {
                try {
                    std::thread th(handleClient, isLargeVariation, hostname, port, client_bucket_size);
                    th.detach();
                }
                catch (GeneralException & e) {
                    std::cerr << e.what() << "\n";
                    continue;
                }
            }
        }
    }

    // 4 parameters, run server side code
    else {
        if (checkServerOrClient != 's') {
            std::cerr << "Invalid parameter [1], please enter s to run client side codes." <<
            std::endl;
            return EXIT_FAILURE;            
        }
        // run server codes
        else {
            std::cout << "Server side number of buckets is: " << argv[3] << std::endl;
            // run server side codes
            std::string checkThreadPool = argv[2];
            int checkBucketSize = std::atoi(argv[3]);
            switch (checkBucketSize) {
                case 1:
                    bucket_size = 32;
                    break;
                case 2:
                    bucket_size = 128;
                    break;
                case 3:
                    bucket_size = 512;
                    break;
                case 4:
                    bucket_size = 2048;
                    break;
                default:
                    std::cerr << "Invalid parameter [4], please enter 1 for 32 buckets, 2 for 128 buckets, "
                    "3 for 512 buckets, 4 for 2048 buckets.";
                    return EXIT_FAILURE;
            }

            // server socket initialization
            ServerSocket serverSocket;
            serverSocket.hostname = NULL;
            serverSocket.port = "12345";
            serverSocket.setup(); 

            std::vector<double> bucket;
            bucket.resize(bucket_size);
            int count = 0, request_id = 0;

            // thread per request
            if (checkThreadPool == "0") {
                int times = 0;
                struct timeval start;
                while (1) {
                    ++request_id;
                    int client_fd = -1;
                    try {
                        client_fd = prepareServer(serverSocket);
                        // start count time
                        ++times;
                        if (times == 1) {
                            gettimeofday(&start, NULL);
                        }
                    }
                    catch (GeneralException & e) {
                        closeSockfd(client_fd);
                        // freeSockAddrList(serverSocket.host_info_list);
                        std::cerr << e.what() << "\n";
                        continue;
                    }
                    try {
                        std::thread th(handleServer, 
                        client_fd, std::ref(count), std::ref(bucket), std::ref(start));
                        th.detach();
                    }
                    catch (GeneralException & e) {
                        closeSockfd(client_fd);
                        // freeSockAddrList(serverSocket.host_info_list);
                    }
                }
            }
            // thread pool
            else if (checkThreadPool == "1") {
                std::threadpool tp(THREAD_SIZE);
                int times = 0;
                struct timeval start;
                while (1) {
                    ++request_id;
                    int client_fd = -1;
                    try {
                        client_fd = prepareServer(serverSocket);
                        // start count time
                        ++times;
                        if (times == 1) {
                            gettimeofday(&start, NULL);
                        }
                    }
                    catch (GeneralException & e) {
                        closeSockfd(client_fd);
                        // freeSockAddrList(serverSocket.host_info_list);
                        std::cerr << e.what() << "\n";
                        continue;
                    }
                    try {
                        tp.commit(handleServer, 
                        client_fd, std::ref(count), std::ref(bucket), std::ref(start));
                    }
                    catch (std::exception & e) {
                        closeSockfd(client_fd);
                        // freeSockAddrList(serverSocket.host_info_list);
                        std::cerr << e.what() << "\n";
                    }
                }

                // std::vector<std::thread> threadPool;
                // std::queue<int> requestQueue;
                // // TODO: thread pool size should be specified by user
                // for (int i = 0; i < 256; ++i) {
                //     std::thread th(handleServerThreadpool, request_id, 
                //     std::ref(requestQueue), std::ref(count), std::ref(bucket));
                //     threadPool.push_back(th);
                //     th.join();
                // }
                // while (1) {
                //     ++request_id;
                //     int client_fd = -1;
                //     // accept request and push that into queue
                //     client_fd = prepareServer(serverSocket);
                //     try {
                //         std::lock_guard<std::mutex> lck(mtx);
                //         requestQueue.push(client_fd);
                //     }
                //     catch(GeneralException & e) {
                //         throw GeneralException("lock guard failed.");
                //     } 
                // }
            }
            else {
                std::cerr << "Invalid parameter [3], please enter 1 to run thread pool or 0 to run thread per request.\n";
                return EXIT_FAILURE;
            }
        }
    }
}