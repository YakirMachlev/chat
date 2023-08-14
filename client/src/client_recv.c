#include "client_recv.h"

static void handle_recv_from_server(char *buffer, int length)
{
    response_e response;
    
    response = (uint8_t)*(buffer++);
    printf("response: %d\n", response);
    switch (response)
    {
    case REGISTER_RESPONSE:
        client_responses_register(buffer);
        break;
    case LOGIN_RESPONSE:
        client_responses_login(buffer);
        break;
    case LIST_ROOMS_RESPONSE:
        client_responses_list_rooms(buffer);
        break;
    case JOIN_ROOM_RESPONSE:
        client_responses_join_room(buffer);
        break;
    case SEND_MESSAGE_IN_ROOM_RESPONSE:
        client_responses_send_message_in_room(buffer);
        break;
    case EXIT_ROOM_RESPONSE:
        client_responses_exit_room(buffer);
        break;
    case SEND_SERVER_MESSAGE_IN_ROOM:
        client_responses_server_send_message_in_room(buffer);
        break;
    default:
        puts("Invalid response.");
        close(client.sockfd);
        exit(1);
        break;
    }
}

void *receive_data_from_server(void *arg)
{
    int sockfd;
    char buffer[DATA_MAX_LENGTH];
    int bytes_received;

    pthread_detach(pthread_self());
    sockfd = *((int *)arg);
    while (true)
    {
        bytes_received = recv(sockfd, buffer, DATA_MAX_LENGTH, 0);
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
            {
                printf("server: socket %d hung up\n", sockfd);
            }
            else
            {
                perror("recv");
            }
            close(sockfd);
            exit(1);
            break;
        }
        buffer[bytes_received] = '\0';
        handle_recv_from_server(buffer, bytes_received);
    }

    return NULL;
}
