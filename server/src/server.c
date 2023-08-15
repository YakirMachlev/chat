#include "server_client.h"

int connected_clients;

void server_end_client_connection(int sockfd)
{
    uint8_t total_length;
    char buffer[22];
    uint8_t length;
    request_e request;
    
    request = SEND_SERVER_MESSAGE_IN_ROOM;
    length = strlen(SERVER_FULL_MSG);
    total_length = sprintf(buffer, "%c%c%s", request, length, SERVER_FULL_MSG);
    send(sockfd, buffer, total_length, 0);
}

void *handle_clients(void *arg)
{
    int client_temp_sockfd;
    void *client_sockfd;
    pthread_t client_thread;
    int server_sockfd;

    pthread_detach(pthread_self());

    server_sockfd = *(int *)arg;
    while (true)
    {
        client_sockfd = (int *)malloc(sizeof(int));
        client_temp_sockfd = accept(server_sockfd, NULL, NULL);
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
            server_end_client_connection(client_temp_sockfd);
        }
        else
        {
            pthread_create(&client_thread, NULL, server_client_handle_functions, client_sockfd);
            pthread_detach(client_thread);
        }
    }
    return NULL;
}

int server_find_valid_address(struct addrinfo *servinfo)
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

int main()
{
    int server_sockfd;
    int rv;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    pthread_t accept_thread;

    init_chat_rooms();
    connected_clients = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "server: %s\n", gai_strerror(rv));
        exit(1);
    }
    server_sockfd = server_find_valid_address(servinfo);

    if (listen(server_sockfd, NUM_OF_CONNECTIONS) == -1)
    {
        perror("listen");
        exit(1);
    }

    puts("Waiting for connections");

    pthread_create(&accept_thread, NULL, handle_clients, (void *)&server_sockfd);
    pthread_join(accept_thread, NULL);

    return 0;
}
