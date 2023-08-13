#pragma once

#define __USE_XOPEN2K
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include "client_conf.h"

#define INVALID_ACTION "Invalid action"
#define INVALID_NAME "Invalid name"
#define WRONG_PASSWORD "Wrong password"
#define NO_CLIENT "The client with the given name doesn't exist"
#define NOT_CONNECTED "The client with the given name isn't connected"
#define CLIENT_CANT_JOIN "Client already in another room / not connected"

void client_requests_register(int sockfd);
void client_requests_login(int sockfd);
void client_requests_list_rooms(int sockfd);
void client_requests_join_room(int sockfd);
void client_requests_send_message_in_room(int sockfd, char *msg, uint16_t msg_length);
