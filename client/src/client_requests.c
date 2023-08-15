#include "client_requests.h"

static bool client_requests_check_name_validity(char *name)
{
    uint8_t offset;
    bool valid;

    valid = true;
    for (offset = 0; offset < NAME_MAX_LENGTH && *name; offset++)
    {
        if (!isalnum(*(name++)))
        {
            valid = false;
        }
    }
    if (!valid)
    {
        puts("Name contains charcters that are not alpha-numeric");
    }
    else if (*name)
    {
        puts("Name too long");
        valid = false;
    }
    return valid;
}

static void client_requests_start(int sockfd, request_e request)
{
    char buffer[CLIENT_DATA_LENGTH];
    char password[PASSWORD_MAX_LENGTH];
    uint8_t password_length;
    uint8_t total_length;
    bool is_name_valid;

    is_name_valid = false;
    while (!is_name_valid)
    {
        printf("Enter your name (max 12 alpha-numeric characters):\n> ");
        scanf("%s", client.name);
        is_name_valid = client_requests_check_name_validity(client.name);
    }
    client.name_length = strlen(client.name);

    printf("Enter your password (max 32 characters):\n> ");
    scanf("%s", password);
    password_length = strlen(password);

    total_length = sprintf(buffer, "%c%c%s%c%s", request, client.name_length, client.name, password_length, password);
    send(sockfd, buffer, total_length, 0);
}

void client_requests_register(int sockfd)
{
    client_requests_start(sockfd, REGISTER_REQUEST);
}

void client_requests_login(int sockfd)
{
    client_requests_start(sockfd, LOGIN_REQUEST);
}

void client_requests_list_rooms(int sockfd)
{
    char request;
    request = LIST_ROOMS_REQUEST;
    send(sockfd, &request, 1, 0);
}

void client_requests_join_room(int sockfd)
{
    char buffer[NAME_MAX_LENGTH + 3];
    uint8_t total_length;
    request_e request;

    request = JOIN_ROOM_REQUEST;
    total_length = sprintf(buffer, "%c%c%s%c", request, client.name_length, client.name, client.room_id);
    send(sockfd, buffer, total_length, 0);
}

void client_requests_send_message_in_room(int sockfd, char *msg, uint16_t msg_length)
{
    char buffer[DATA_MAX_LENGTH];
    uint16_t total_length;
    request_e request;

    request = SEND_MESSAGE_IN_ROOM_REQUEST;
    total_length = sprintf(buffer, "%c%c%s%hd%s", request, client.name_length, client.name, msg_length, msg);
    send(sockfd, buffer, total_length, 0);
}

void client_requests_exit_room(int sockfd)
{
    char buffer[NAME_MAX_LENGTH + 2];
    uint8_t total_length;
    request_e request;

    request = EXIT_ROOM_REQUEST;
    total_length = sprintf(buffer, "%c%c%s", request, client.name_length, client.name);
    send(sockfd, buffer, total_length, 0);
}