#include "server_client.h"

int connected_clients;

#define SERVER_FULL_MSG "server currently full"

#define EXIT_PROGRAM(msg)         \
    fprintf(stderr, "%s\n", msg); \
    exit(1);

void *handle_clients(void *arg)
{
    int client_temp_sockfd;
    void *client_sockfd;
    pthread_t client_thread;
    char buffer[24];
    uint8_t total_length;
    int listener;

    pthread_detach(pthread_self());
    total_length = sprintf(buffer, "%c%c%21s", (uint8_t)SEND_SERVER_MESSAGE_IN_ROOM_RESPONSE, 21, SERVER_FULL_MSG);
    
    listener = *(int *)arg;
    while (true)
    {
        client_sockfd = (int *)malloc(sizeof(int));
        client_temp_sockfd = accept(listener, NULL, NULL);
        connected_clients++;
        printf("Client %d connected\n", connected_clients);

        if (client_temp_sockfd == -1)
        {
            perror("accept");
            continue;
        }

        *(int *)client_sockfd = client_temp_sockfd;
        if (connected_clients > NUM_OF_CONNECTIONS)
        {
            send(client_temp_sockfd, buffer, total_length, 0);
            connected_clients--;
            close(client_temp_sockfd);
        }
        else
        {
            pthread_create(&client_thread, NULL, server_client_handle_functions, client_sockfd);
            pthread_detach(client_thread);
        }
    }
    return NULL;
}

/**
 * @brief finds a valid socket for the server according to the
 * information specified in the given server info
 *
 * @param servinfo the server info
 * @return int the server socket file descriptor
 */
static int server_find_valid_socket(struct addrinfo *servinfo)
{
    int server_socket;
    struct addrinfo *p;
    int yes = 1;

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(server_socket);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    return server_socket;
}

/**
 * @brief gets the server's listener socket file descriptor
 *
 * @return int listener socket file descriptor on success, else NULL
 */
static int server_get_listener_socket()
{
    int listener;
    struct addrinfo hints;
    struct addrinfo *server_info;
    int8_t err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((err = getaddrinfo(NULL, PORT, &hints, &server_info)))
    {
        EXIT_PROGRAM(gai_strerror(err))
    }
    listener = server_find_valid_socket(server_info);
    if (listen(listener, NUM_OF_CONNECTIONS + 1) == -1)
    {
        EXIT_PROGRAM("Error getting listening socket")
    }

    return listener;
}

int main()
{
    int listener;
    pthread_t accept_thread;

    init_chat_rooms();
    connected_clients = 0;

    listener = server_get_listener_socket();
    puts("Waiting for connections");

    pthread_create(&accept_thread, NULL, handle_clients, (void *)(&listener));
    pthread_join(accept_thread, NULL);

    return 0;
}
