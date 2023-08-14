#include "client_ui.h"

void client_ui_first_hierarchy(int sockfd);
void client_ui_second_hierarchy(int sockfd);
void client_ui_third_hierarchy(int sockfd);

#define CALL_HIERARCHY(hierarchy, sockfd)   \
    switch (hierarchy)                      \
    {                                       \
    case 1:                                 \
        client_ui_first_hierarchy(sockfd);  \
        break;                              \
    case 2:                                 \
        client_ui_second_hierarchy(sockfd); \
        break;                              \
    case 3:                                 \
        client_ui_third_hierarchy(sockfd);  \
        break;                              \
    case 4:                                 \
        close(sockfd);                      \
        exit(1);                            \
        break;                              \
    case 5:                                 \
        break;                              \
    }

/* hierarchies */

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

    if (option == 1 || option == 2)
    {
        pthread_mutex_lock(&mutex);
        is_received = false;

        option == 1 ? client_requests_register(sockfd) : client_requests_login(sockfd);
        while (!is_received)
            pthread_cond_wait(&cond, &mutex);
        CALL_HIERARCHY(hierarchy, sockfd)
        pthread_mutex_unlock(&mutex);
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

    pthread_mutex_lock(&mutex);
    is_received = false;
    client_requests_list_rooms(sockfd);
    while (!is_received)
        pthread_cond_wait(&cond, &mutex);
    CALL_HIERARCHY(hierarchy, sockfd)
    room_num = -1;
    while (room_num < 1 || room_num > 5)
    {
        printf("Choose which room you want to join\n> ");
        scanf("%d", &room_num);
    }

    client.room_id = room_num;
    is_received = false;
    client_requests_join_room(sockfd);
    while (!is_received)
        pthread_cond_wait(&cond, &mutex);
    CALL_HIERARCHY(hierarchy, sockfd)
    pthread_mutex_unlock(&mutex);
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
        is_received = false;
        client_requests_exit_room(sockfd);
        while (!is_received)
            pthread_cond_wait(&cond, &mutex);
        CALL_HIERARCHY(hierarchy, sockfd)
        pthread_mutex_unlock(&mutex);
        close(sockfd);
        exit(0);
    }
}