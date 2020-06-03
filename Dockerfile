FROM ubuntu:18.04
RUN apt-get update && apt-get install -y g++
RUN mkdir /code
WORKDIR /code
ADD . /code/