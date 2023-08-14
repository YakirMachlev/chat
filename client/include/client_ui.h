#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include "client_requests.h"

void *client_ui_start(void* arg);
