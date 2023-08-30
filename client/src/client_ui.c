#include "client_ui.h"

void client_ui_first_hierarchy(int sockfd);
void client_ui_second_hierarchy(int sockfd);
void client_ui_third_hierarchy(int sockfd);

#define RUN_ACTION(hierarchy, sockfd)       \
    switch (action)                         \
    {                                       \
    case FIRST_HIERARCHY:                   \
        client_ui_first_hierarchy(sockfd);  \
        break;                              \
    case SECOND_HIERARCHY:                  \
        client_ui_second_hierarchy(sockfd); \
        break;                              \
    case THIRD_HIERARCHY:                   \
        client_ui_third_hierarchy(sockfd);  \
        break;                              \
    case EXIT:                              \
        close(sockfd);                      \
        exit(1);                            \
        break;                              \
    case NONE:                              \
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

    pthread_exit(NULL);
}

void client_ui_first_hierarchy(int sockfd)
{
    char option;

    printf("choose 1 - registration / 2 - login\n> ");
    scanf(" %c", &option);

    if (option == '1' || option == '2')
    {
        pthread_mutex_lock(&mutex);
        is_received = false;

        option == '1' ? client_requests_register(sockfd) : client_requests_login(sockfd);
        while (!is_received)
            pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
        RUN_ACTION(action, sockfd)
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
    pthread_mutex_unlock(&mutex);
    RUN_ACTION(action, sockfd)
    room_num = -1;
    while (room_num < 1 || room_num > num_of_rooms)
    {
        printf("Choose which room you want to join (1-%d)\n> ", num_of_rooms);
        scanf("%d", &room_num);
    }
    pthread_mutex_lock(&mutex);
    client.room_id = room_num;
    is_received = false;
    client_requests_join_room(sockfd);
    while (!is_received)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    RUN_ACTION(action, sockfd)
}

void client_ui_third_hierarchy(int sockfd)
{
    char buffer[CLEAR_DATA_MAX_LENGTH];
    uint8_t buffer_length;

    fgets(buffer, CLEAR_DATA_MAX_LENGTH, stdin);
    fgets(buffer, CLEAR_DATA_MAX_LENGTH, stdin);
    while (strncmp(buffer, "~`\n", 3))
    {
        buffer_length = strlen(buffer);
        client_requests_send_message_in_room(sockfd, buffer, buffer_length);
        fgets(buffer, CLEAR_DATA_MAX_LENGTH, stdin);
    }

    pthread_mutex_lock(&mutex);
    is_received = false;
    client_requests_exit_room(sockfd);
    while (!is_received)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    RUN_ACTION(action, sockfd)
}