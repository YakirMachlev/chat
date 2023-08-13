#include "client_ui.h"

/* hierarchies ui */

void *client_ui_start(void *arg)
{
    int sockfd;
    pthread_detach(pthread_self());

    sockfd = *(int *)arg;
    client.sockfd = sockfd;
    client_ui_first_hierarchy(sockfd);

    return NULL;
}

void client_ui_first_hierarchy(int sockfd)
{
    int option;
    printf("choose 1 - registration / 2 - login\n> ");
    scanf("  %d", &option);

    if (option == 1)
    {
        client_requests_register(sockfd);
        pthread_cond_wait(&condition, &lock);
    }
    else if (option == 2)
    {
        client_requests_login(sockfd);
        pthread_cond_wait(&condition, &lock);
    }
    else
    {
        puts("Invalid option");
        client_ui_first_hierarchy(sockfd);
    }
}

void client_ui_second_hierarchy(int sockfd)
{
    int room_num;

    puts("a");
    client_requests_list_rooms(sockfd);
    puts("b");
    pthread_cond_wait(&condition, &lock);
    puts("c");
    room_num = -1;
    while (room_num < 1 || room_num > 5)
    {
        printf("Choose which room you want to join\n> ");
        scanf("%d", &room_num);
    }

    client.room_id = room_num;
    client_requests_join_room(sockfd);
    pthread_cond_wait(&condition, &lock);
}

void client_ui_third_hierarchy(int sockfd)
{
    char buffer[CLEAR_DATA_MAX_LENGTH];
    uint16_t buffer_length;

    *buffer = '-';
    while (strncmp(buffer, "~`", 3))
    {
        printf("\n> ");
        scanf("%s", buffer);
        buffer_length = strlen(buffer);
        client_requests_send_message_in_room(sockfd, buffer, buffer_length);
    }
    if (strncmp(buffer, "~`", 3))
    {
        puts("Exiting the program");
        close(sockfd);
        exit(0);
    }
}

/* responses ui */

void client_ui_register_response(char *buffer)
{
    int8_t result;

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
        pthread_cond_signal(&condition);
        client_ui_first_hierarchy(client.sockfd);
    }
    else
    {
        puts("Invalid response");
    }
}

void client_ui_login_response(char *buffer)
{
    int8_t result;

    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Login succeded");
        pthread_cond_signal(&condition);
        client_ui_second_hierarchy(client.sockfd);
    }
    else if (result == -1 && !*buffer)
    {
        puts("Wrong login details");
        pthread_cond_signal(&condition);
        client_ui_first_hierarchy(client.sockfd);
    }
    else
    {
        puts("Invalid response");
    }
}

void client_ui_list_rooms_response(char *buffer)
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

void client_ui_join_room_response(char *buffer)
{
    int8_t result;

    result = *buffer;
    if (result == 0 && !*buffer)
    {
        puts("Join succeded");

        pthread_cond_signal(&condition);
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

void client_ui_send_message_in_room_response(char *buffer)
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

void client_ui_exit_room_response(char *buffer)
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

void client_ui_server_send_message_in_room_response(char *buffer)
{
    char *msg;
    uint16_t msg_length;

    msg_length = *(buffer++) << 8;
    msg_length |= *(buffer++);
    msg = buffer;
    msg[msg_length] = '\0';

    printf("server: %s\n", msg);
}