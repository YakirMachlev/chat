#include "client_responses.h"

/* responses */

void client_responses_register(char *buffer)
{
    int8_t result;

    pthread_mutex_lock(&mutex);
    result = *(buffer++);
    if (result == 0 && !*buffer)
    {
        puts("Registration succeded\n");
        action = FIRST_HIERARCHY;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Registration failed\n");
        action = FIRST_HIERARCHY;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        action = EXIT;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_login(char *buffer)
{
    int8_t result;

    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Login succeded\n");
        action = SECOND_HIERARCHY;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Wrong login details\n");
        action = FIRST_HIERARCHY;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        action = EXIT;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_list_rooms(char *buffer)
{
    int8_t result;
    uint8_t offset;

    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == -1)
    {
        puts("List rooms failed");
        action = SECOND_HIERARCHY;
    }
    else
    {
        num_of_rooms = result;
        for (offset = 1; offset <= num_of_rooms; offset++)
        {
            printf("room #%d: %d\n", offset, *(++buffer));
        }
        action = NONE;
    }
    puts("");
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_join_room(char *buffer)
{
    int8_t result;
    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Join succeded\n");
        action = THIRD_HIERARCHY;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Join failed\n");
        client.room_id = -1;
        action = SECOND_HIERARCHY;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        action = EXIT;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_send_message_in_room(char *buffer)
{
    char *name;
    char *msg;
    uint8_t name_length;
    uint8_t msg_length;

    name_length = *(buffer++);
    name = buffer;

    buffer += name_length;
    msg_length = *(buffer++);
    msg = buffer;

    name[name_length] = '\0';
    msg[msg_length] = '\0';

    printf("%s: %s", name, msg);
}

void client_responses_exit_room(char *buffer)
{
    int8_t result;

    pthread_mutex_lock(&mutex);
    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Exit room succeded\n");
        action = SECOND_HIERARCHY;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Exit room failed\n");
        action = THIRD_HIERARCHY;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        action = EXIT;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_server_send_message_in_room(char *buffer)
{
    char *msg;
    uint8_t msg_length;

    msg_length = *(buffer++);
    msg = buffer;
    msg[msg_length] = '\0';

    printf("*server massage*: %s\n", msg);
}