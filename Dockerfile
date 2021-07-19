FROM ubuntu:18.04

RUN apt-get -y update
RUN apt-get -y upgrade
RUN apt-get -y install \
	gcc \
	make \
	libsdl2-dev \
	libsdl2-image-dev \
	libsdl2-ttf-dev \
	libfreetype6-dev \
	fonts-dejavu

WORKDIR app
COPY . .
RUN make
