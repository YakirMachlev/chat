#include "client_responses.h"

/* responses */

void client_responses_register(char *buffer)
{
    int8_t result;

    pthread_mutex_lock(&mutex);
    result = *(buffer++);
    if (result == 0 && !*buffer)
    {
        puts("Registration succeded");
        hierarchy = 1;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Registration failed");
        hierarchy = 1;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        hierarchy = 4;
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
        puts("Login succeded");
        hierarchy = 2;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Wrong login details");
        hierarchy = 1;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        hierarchy = 4;
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
        hierarchy = 2;
    }
    else
    {
        for (offset = 1; offset <= result; offset++)
        {
            printf("room #%d: %d\n", offset, *(++buffer));
        }
        hierarchy = 5;
    }
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
        puts("Join succeded");
        hierarchy = 3;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Join failed");
        hierarchy = 2;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        hierarchy = 4;
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
    uint16_t msg_length;

    name_length = *(buffer++);
    name = buffer;

    buffer += name_length;
    msg_length = *(buffer++) << 8;
    msg_length |= *(buffer++);
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
        puts("Exit room succeded");
        hierarchy = 2;
    }
    else if (result == -1 && !*buffer)
    {
        puts("Exit room failed");
        hierarchy = 3;
    }
    else
    {
        puts("Invalid response. Shuting down the connection");
        hierarchy = 4;
    }
    is_received = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void client_responses_server_send_message_in_room(char *buffer)
{
    char *msg;
    uint16_t msg_length;

    msg_length = *(buffer++) << 8;
    msg_length |= *(buffer++);
    msg = buffer;
    msg[msg_length] = '\0';

    printf("*server*: %s\n", msg);
}