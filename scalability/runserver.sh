#!/bin/bash

# To choose thread per request, set THREAD_FORM to 0
# To choose thread pool, set THREAD_FORM to 1
# To choose number of buckets to 32, 128, 512, 2048, set NUM_BUCKETS to 1, 2, 3, 4 respectively
# To select the CPU resource, set NUM_CORES, 0 for 1 core, 0-1 for 2 cores, 0-3 for 4 cores etc.
THREAD_FORM="1"
NUM_BUCKETS="4"
NUM_CORES="0-3"

# print commands
# echo "make clean"
# echo "make"
# echo "taskset -c NUM_CORES ./scalaMain s $THREAD_FORM $NUM_BUCKETS"

# run commands
make clean
make
taskset -c $NUM_CORES ./scalaMain s $THREAD_FORM $NUM_BUCKETS