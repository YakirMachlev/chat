#pragma once

#include <inttypes.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "1234"
#define NAME_MAX_LENGTH 12
#define PASSWORD_MAX_LENGTH 32
#define DATA_MAX_LENGTH (1 << (sizeof(uint8_t) * 8))
#define CLIENT_DATA_LENGTH NAME_MAX_LENGTH + PASSWORD_MAX_LENGTH + 4
#define CLEAR_DATA_MAX_LENGTH DATA_MAX_LENGTH - NAME_MAX_LENGTH - 3

typedef struct
{
    int sockfd;
    char name[NAME_MAX_LENGTH];
    uint8_t name_length;
    int8_t room_id;
} client_t;

typedef enum
{
    FIRST_HIERARCHY = 1,
    SECOND_HIERARCHY,
    THIRD_HIERARCHY,
    EXIT,
    NONE
} actions_e;

extern client_t client;
extern pthread_cond_t cond;
extern pthread_mutex_t mutex;
extern bool is_received;
extern actions_e action;
extern uint8_t num_of_rooms;

typedef enum
{
    REGISTER_REQUEST = 100,
    LOGIN_REQUEST,
    LIST_ROOMS_REQUEST,
    JOIN_ROOM_REQUEST,
    SEND_MESSAGE_IN_ROOM_REQUEST,
    EXIT_ROOM_REQUEST
} request_e;

typedef enum
{
    REGISTER_RESPONSE = 200,
    LOGIN_RESPONSE,
    LIST_ROOMS_RESPONSE,
    JOIN_ROOM_RESPONSE,
    SEND_MESSAGE_IN_ROOM_RESPONSE,
    EXIT_ROOM_RESPONSE,
    SEND_SERVER_MESSAGE_IN_ROOM_RESPONSE = 211
} response_e;