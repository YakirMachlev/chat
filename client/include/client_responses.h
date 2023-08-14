#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "client_conf.h"

void client_responses_register(char *buffer);
void client_responses_login(char *buffer);
void client_responses_list_rooms(char *buffer);
void client_responses_join_room(char *buffer);
void client_responses_send_message_in_room(char *buffer);
void client_responses_exit_room(char *buffer);
void client_responses_server_send_message_in_room(char *buffer);
