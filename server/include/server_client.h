#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <sys/socket.h>
#include "server_conf.h"
#include "server_client_functions.h"

void *server_client_handle_functions(void *arg);