# Scalability (HW4 for ECE 568 ERSS 2020 Spring)

By Qing Lu, Chixiang Zhang, Yijun Mao

## Introduction

In order to simulate the scenario of a server handling multiple client requests and test its performance and scalability, we create client and server programs. To run the program, first you need to open two terminals, one is for the server and one is for the client.
server has three parameters:

- The first parameter “s” indicates it runs as server
- The second parameter indicates the way of creating threads. “0”: it uses thread-per-request, “1”: it uses thread pool method to handle coming requests.
- The third parameter indicates how many buckets you want to have for the server side. 1 for 32 buckets, 2 for 128 buckets, 3 for 512 buckets, 4 for 2048 buckets.

client has four parameters:
- The first parameter “c” indicates it runs as client
- The second parameter indicates how many request will be handled at the same time
- The third number represents whether it has small delay variation or large delay variation
- The fourth parameter indicates how many buckets you want to have for the server side. 1 for 32 buckets, 2 for 128 buckets, 3 for 512 buckets, 4 for 2048 buckets.

In this project, we used two methods to handle the request. One is creating thread per request. Once the request is received from the server, the server will create one thread to handle it. The other is creating multiple threads in a threadpool in advance. We utilized lzpong’s thread pool library (https://github.com/lzpong/threadpool) to realize it. The capacity of our thread pool was set to 512. Also, in order to evaluate the throughput of the two methods, we used a variable named count to count how many threads has been handled by the server. When computing the elapsed time, we set up a timer which starts when the server first time successfully accepts a request from the client. The throughput could then be calculated by (count) / (current time - start time).

Next, we will discuss how the number of cores, buckets and requests, and variations in delay impact performance (throughput). The performance might be affected by other factors, like network speed and virtual machines performance . In the assignment, we rule out outside factors.

## Usage

Please refer to the version with the latest finalize commit for grading.

To run server side code, plese type:

```
sudo docker-compose up
```

If docker does not work, plese type:

```
cd scalability
bash runserver.sh
```

to run server code. 

You can adjust the thread strategy (thread per request, thread pool), bucket size, or CPU core number in the server side by alternating the values of ```THREAD_FORM```, ```NUM_BUCKETS``` and ```NUM_CORES``` in ```runserver.sh```, respectively, the instructions of how to modify the parameters are written in ```runserver.sh```.

You must run server side code first (keep the server running) before running client side code!

To run client side code, please type:

```
cd testing
bash runclient.sh
```

You can adjust the number of requests, variation type, or bucket size in the client side by alternating the values of ```NUM_REQUESTS```, ```VARIATION_TYPE``` and ```NUM_BUCKETS``` in ```runclient.sh```, respectively, the instructions of how to modify the parameters are written in ```runclient.sh```.