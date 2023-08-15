#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include "server_conf.h"

#define NUM_OF_ROOMS 5

typedef struct room_t room_t;

void init_chat_rooms();
bool rooms_is_room_available(uint8_t room_id);
void add_client_to_room(client_t *client, uint8_t room_num);
void remove_client_from_room(client_t *client);
void get_rooms_list(client_t *client, uint8_t *rooms_list);
void send_message_to_room(client_t *client, char *msg, int len);