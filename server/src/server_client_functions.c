#include "server_client_functions.h"

#define SEND_RESULT(client, error, opcode, result) \
    error[0] = opcode;                       \
    error[1] = result;                           \
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
    }
    else
    {
        SEND_RESULT(client, error, REGISTER_RESPONSE, -1);
    }
}

void client_login(client_t *client, uint8_t *buffer)
{
    char *name;
    char *password;
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t password_length;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    name = (char *)buffer;

    buffer += name_length;
    password_length = *(buffer++);
    ASSERT(password_length < PASSWORD_MAX_LENGTH && password_length > 0)
    password = (char *)buffer;

    name[name_length] = '\0';
    password[password_length] = '\0';
    ASSERT(check_name_validity(name))

    if (client->state == EXISTS && client_file_check_client_validity(name, password))
    {
        client->state = CONNECTED;
        SEND_RESULT(client, error, LOGIN_RESPONSE, 0);
    }
    else
    {
        SEND_RESULT(client, error, LOGIN_RESPONSE, -1);
    }
}

void client_list_rooms(client_t *client)
{
    uint8_t rooms_list[NUM_OF_ROOMS + 2];
    uint8_t error[ERROR_LENGTH];

    rooms_list[0] = LIST_ROOMS_RESPONSE;
    rooms_list[1] = NUM_OF_ROOMS;
    if (client->state == CONNECTED)
    {
        get_rooms_list(client, rooms_list + 2);
        send(client->sockfd, rooms_list, sizeof(rooms_list) / sizeof(uint8_t), 0);
    }
    else
    {
        SEND_RESULT(client, error, LIST_ROOMS_RESPONSE, -1);
    }
}

void client_join_room(client_t *client, uint8_t *buffer)
{
    uint8_t room_num;
    char connection_msg[NAME_MAX_LENGTH + 11];
    uint8_t error[ERROR_LENGTH];
    uint8_t name_length;
    uint8_t connection_msg_len;

    name_length = *(buffer++);
    ASSERT(name_length < NAME_MAX_LENGTH && name_length > 0)
    buffer += name_length;
    room_num = *buffer;
    ASSERT(room_num <= 5 && room_num >= 1)
    room_num--; /* assuming that the client isn't aware that arrays start at 0 */
    
    if (client->state == CONNECTED)
    {
        connection_msg_len = sprintf(connection_msg, "%s connected.", client->name);
        send_message_to_room(room_num, connection_msg, connection_msg_len);
        add_client_to_room(client, room_num);
        client->state = JOINED;
        client->room_id = room_num;
        SEND_RESULT(client, error, JOIN_ROOM_RESPONSE, 0);
    }
    else
    {
        SEND_RESULT(client, error, JOIN_ROOM_RESPONSE, -1);
    }
}

void client_send_massage_in_room(client_t *client, uint8_t *buffer, int length)
{
    *buffer = SEND_MESSAGE_IN_ROOM_RESPONSE;
    if (client->state == JOINED)
    {
        send_message_to_room(client->room_id, (char *)buffer, length);
    }
}

void client_exit_room(client_t *client)
{
    uint8_t error[ERROR_LENGTH];
    char disconnection_msg[NAME_MAX_LENGTH + 14];
    uint8_t disconnection_msg_len;
    if (client->state == JOINED)
    {
        remove_client_from_room(client);
        disconnection_msg_len = sprintf(disconnection_msg, "%s disconnected.", client->name);
        send_message_to_room(client->room_id, disconnection_msg, disconnection_msg_len);
        client->state = CONNECTED;
        client->room_id = -1;
        SEND_RESULT(client, error, EXIT_ROOM_RESPONSE, 0);
    }
    else
    {
        SEND_RESULT(client, error, EXIT_ROOM_RESPONSE, -1);
    }
}