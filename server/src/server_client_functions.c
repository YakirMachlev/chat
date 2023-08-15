#include "server_client_functions.h"

#define SEND_RESULT(client, error, opcode, result) \
    error[0] = opcode;                             \
    error[1] = result;                             \
    send(client->sockfd, error, ERROR_LENGTH, 0);

void client_register(client_t *client, uint8_t *buffer)
{
    char *password;
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t password_length;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    strncpy(client->name, (char *)buffer, name_length);
    ASSERT(check_name_validity(client->name))

    buffer += name_length;
    password_length = *(buffer++);
    ASSERT(password_length < PASSWORD_MAX_LENGTH && password_length > 0)
    password = (char *)buffer;
    password[password_length] = '\0';

    if (client->state == EXISTS && !client_file_does_client_exist(client->name))
    {
        insert_client_to_file(client->name, password);
        printf("%s registered\n", client->name);
        SEND_RESULT(client, error, REGISTER_RESPONSE, 0);
        printf("sent: %d,%d\n", REGISTER_RESPONSE, 0);
    }
    else
    {
        SEND_RESULT(client, error, REGISTER_RESPONSE, -1);
        printf("sent: %d,%d\n", REGISTER_RESPONSE, -1);
    }
}

void client_login(client_t *client, uint8_t *buffer)
{
    char *password;
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t password_length;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    strncpy(client->name, (char *)buffer, name_length);
    ASSERT(check_name_validity(client->name))

    buffer += name_length;
    password_length = *(buffer++);
    ASSERT(password_length < PASSWORD_MAX_LENGTH && password_length > 0)
    password = (char *)buffer;
    password[password_length] = '\0';

    if (client->state == EXISTS && client_file_check_client_validity(client->name, password))
    {
        client->state = CONNECTED;
        SEND_RESULT(client, error, LOGIN_RESPONSE, 0);
        printf("sent: %d,%d\n", LOGIN_RESPONSE, 0);
    }
    else
    {
        SEND_RESULT(client, error, LOGIN_RESPONSE, -1);
        printf("sent: %d,%d\n", LOGIN_RESPONSE, -1);
    }
}

void client_list_rooms(client_t *client)
{
    uint8_t rooms_list[NUM_OF_ROOMS + 2];
    uint8_t error[ERROR_LENGTH];
    int offset;

    rooms_list[0] = LIST_ROOMS_RESPONSE;
    rooms_list[1] = NUM_OF_ROOMS;
    if (client->state == CONNECTED)
    {
        get_rooms_list(client, rooms_list + 2);
        send(client->sockfd, rooms_list, sizeof(rooms_list) / sizeof(uint8_t), 0);

        printf("sent: %d,%d,", rooms_list[0], rooms_list[1]);
        for (offset = 0; offset < NUM_OF_ROOMS; offset++)
        {
            printf("%d ", rooms_list[2 + offset]);
        }
        puts("");
    }
    else
    {
        SEND_RESULT(client, error, LIST_ROOMS_RESPONSE, -1);
        printf("sent: %d,%d\n", LIST_ROOMS_RESPONSE, -1);
    }
}

void client_join_room(client_t *client, uint8_t *buffer)
{
    uint8_t room_num;
    char connection_msg[NAME_MAX_LENGTH + 13];
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t total_length;
    uint8_t connection_msg_len;
    uint8_t response;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    buffer += name_length;
    room_num = *buffer;
    ASSERT(room_num <= 5 && room_num >= 1)
    room_num--; /* assuming that the client doesn't know that arrays start at 0 */

    if (client->state == CONNECTED)
    {
        SEND_RESULT(client, error, JOIN_ROOM_RESPONSE, 0);
        printf("sent: %d,%d\n", JOIN_ROOM_RESPONSE, 0);
        printf("%s joined room %d\n", client->name, room_num + 1);

        add_client_to_room(client, room_num);
        client->state = JOINED;
        client->room_id = room_num;

        response = SEND_SERVER_MESSAGE_IN_ROOM;
        connection_msg_len = name_length + 11;
        total_length = sprintf(connection_msg, "%c%c%s connected.", response, connection_msg_len, client->name);
        client_send_massage_in_room(client, (uint8_t *)connection_msg, (int)total_length);
    }
    else
    {
        SEND_RESULT(client, error, JOIN_ROOM_RESPONSE, -1);
        printf("sent: %d,%d\n", JOIN_ROOM_RESPONSE, -1);
    }
}

void client_send_massage_in_room(client_t *client, uint8_t *buffer, int length)
{
    if (client->state == JOINED)
    {
        send_message_to_room(client, (char *)buffer, length);
        printf("sent: %d,%s\n", *buffer, buffer + 1);
    }
}

void client_exit_room(client_t *client)
{
    uint8_t error[ERROR_LENGTH];
    char disconnection_msg[NAME_MAX_LENGTH + 16];
    uint8_t total_length;
    uint8_t response;
    uint8_t disconnection_msg_length;

    if (client->state == JOINED)
    {
        SEND_RESULT(client, error, EXIT_ROOM_RESPONSE, 0);
        printf("sent: %d,%d\n", EXIT_ROOM_RESPONSE, 0);

        remove_client_from_room(client);
        client->state = CONNECTED;
        client->room_id = -1;

        response = SEND_SERVER_MESSAGE_IN_ROOM;
        disconnection_msg_length = strlen(client->name) + 14;
        total_length = sprintf(disconnection_msg, "%c%c%s disconnected.", response, disconnection_msg_length, client->name);
        client_send_massage_in_room(client, (uint8_t *)disconnection_msg, (int)total_length);
    }
    else
    {
        SEND_RESULT(client, error, EXIT_ROOM_RESPONSE, -1);
        printf("sent: %d,%d\n", EXIT_ROOM_RESPONSE, -1);
    }
}