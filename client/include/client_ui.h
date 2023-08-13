#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include "client_requests.h"

void *client_ui_start(void* arg);
void client_ui_first_hierarchy(int sockfd);

void client_ui_register_response(char *buffer);
void client_ui_login_response(char *buffer);
void client_ui_list_rooms_response(char *buffer);
void client_ui_join_room_response(char *buffer);
void client_ui_send_message_in_room_response(char *buffer);
void client_ui_exit_room_response(char *buffer);
void client_ui_server_send_message_in_room_response(char *buffer);
