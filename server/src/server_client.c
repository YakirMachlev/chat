#include "server_client.h"

#define INIT_CLIENT(client, fd)                  \
    client = (client_t *)malloc(sizeof(client)); \
    client->sockfd = fd;                         \
    client->room_id = -1;                        \
    client->state = EXISTS;

void *server_client_handle_functions(void *arg)
{
    client_t *client;
    int client_sockfd;
    uint8_t opcode;
    uint8_t buffer[DATA_MAX_LENGTH];
    uint8_t *buffer_ptr;
    int bytes_received;

    client_sockfd = *(int *)arg;
    free(arg);
    INIT_CLIENT(client, client_sockfd)

    while (true)
    {
        if ((bytes_received = recv(client_sockfd, buffer, DATA_MAX_LENGTH, 0)) <= 0)
        {
            if (bytes_received == -1)
            {
                perror("recv");
            }
            ASSERT(0)
        }
        else
        {
            buffer_ptr = buffer;
            buffer[bytes_received] = '\0';
            opcode = *(buffer_ptr++);

            printf("request: %d\n", opcode);
            switch (opcode)
            {
            case REGISTER_REQUEST:
                client_register(client, buffer_ptr);
                break;
            case LOGIN_REQUEST:
                client_login(client, buffer_ptr);
                break;
            case LIST_ROOMS_REQUEST:
                client_list_rooms(client);
                break;
            case JOIN_ROOM_REQUEST:
                client_join_room(client, buffer_ptr);
                break;
            case SEND_MESSAGE_IN_ROOM_REQUEST:
                *buffer = SEND_MESSAGE_IN_ROOM_RESPONSE;
                client_send_massage_in_room(client, buffer, bytes_received);
                break;
            case EXIT_ROOM_REQUEST:
                client_exit_room(client);
                break;
            default:
                ASSERT(0)
            }
        }
    }
}