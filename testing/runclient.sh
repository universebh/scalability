#!/bin/bash

# To define number of request, set NUM_REQUESTS
# To choose delay variation type, set VARIATION_TYPE to 0 for small variation, 1 for large variation
# To choose number of buckets to 32, 128, 512, 2048, set NUM_BUCKETS to 1, 2, 3, 4 respectively
NUM_REQUESTS="1024"
VARIATION_TYPE="0"
NUM_BUCKETS="4"

# print commands
# echo "../scalability/scalaMain c $NUM_REQUESTS $VARIATION_TYPE $NUM_BUCKETS"

# run commands
../scalability/scalaMain c $NUM_REQUESTS $VARIATION_TYPE $NUM_BUCKETS