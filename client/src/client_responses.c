#include "client_responses.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
bool is_received = false;

/* responses */

void client_responses_register(char *buffer)
{
    int8_t result;

    pthread_mutex_lock(&lock);
    /* do something that might make condition true */
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
    result = *(buffer++);
    printf("%d\n", result);
    if (!*buffer)
    {
        if (result == 0)
        {
            puts("Registration succeded");
        }
        else if (result == -1)
        {
            puts("Registration failed");
        }
        /* pthread_cond_signal(&condition); */
        client_ui_first_hierarchy(client.sockfd);
    }
    else
    {
        puts("Invalid response");
    }
}

void client_responses_login(char *buffer)
{
    int8_t result;

    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Login succeded");
        /* pthread_cond_signal(&condition); */
        client_ui_second_hierarchy(client.sockfd);
    }
    else if (result == -1 && !*buffer)
    {
        puts("Wrong login details");
        /* pthread_cond_signal(&condition); */
        client_ui_first_hierarchy(client.sockfd);
    }
    else
    {
        puts("Invalid response");
    }
}

void client_responses_list_rooms(char *buffer)
{
    int8_t result;
    uint8_t offset;

    result = *buffer;
    if (result == -1)
    {
        puts("List rooms failed");
    }
    else
    {
        for (offset = 1; offset <= result; offset++)
        {
            printf("room #%d: %d\n", offset, *(++buffer));
        }
    }
}

void client_responses_join_room(char *buffer)
{
    int8_t result;

    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Join succeded");

        pthread_cond_signal(&cond);
        client_ui_third_hierarchy(client.sockfd);
    }
    else if (result == -1 && !*buffer)
    {
        puts("Join failed");
    }
    else
    {
        puts("Invalid response");
    }
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

    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Exit room succeded");
        client_ui_second_hierarchy(client.sockfd);
    }
    else if (result == -1 && !*buffer)
    {
        puts("Exit room failed");
    }
    else
    {
        puts("Invalid response");
    }
}

void client_responses_server_send_message_in_room(char *buffer)
{
    char *msg;
    uint16_t msg_length;

    msg_length = *(buffer++) << 8;
    msg_length |= *(buffer++);
    msg = buffer;
    msg[msg_length] = '\0';

    printf("server: %s\n", msg);
}