#pragma once

#include <inttypes.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "1234"
#define NAME_MAX_LENGTH 13
#define PASSWORD_MAX_LENGTH 33
#define DATA_MAX_LENGTH (1 << (sizeof(uint16_t) * 8))
#define CLIENT_DATA_LENGTH NAME_MAX_LENGTH + PASSWORD_MAX_LENGTH + 3
#define CLEAR_DATA_MAX_LENGTH DATA_MAX_LENGTH - NAME_MAX_LENGTH - 3

#define CLIENT_DISCONNECT     \
    client_exit_room(client); \
    close(client->sockfd);    \
    free(client);             \
    pthread_exit(NULL);

#define ASSERT(expression) \
    if (expression)        \
    {                      \
        CLIENT_DISCONNECT  \
    }

typedef enum
{
    NONE,
    EXISTS,
    CONNECTED,
    JOINED
} state_e;

typedef struct 
{
    int sockfd;
    char name[NAME_MAX_LENGTH];
    uint8_t name_length;
    uint8_t room_id;
    state_e state;
} client_t;

extern client_t client;
extern pthread_cond_t condition;
extern pthread_mutex_t lock;

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
    SEND_SERVER_MESSAGE_IN_ROOM = 211
} response_e;