/*
 * Author : Phuc Le
 *
 * Created on Mon Jan 06 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "control/monitor.h"
#include "communication/comm_manager.h"
#include "control/event_manager.h"

int main() {
    pthread_t monitoring_thread, comm_thread, event_thread;

    pthread_create(&monitoring_thread, NULL, monitoring, NULL);
    pthread_create(&comm_thread, NULL, comm_hanlder, NULL);
    pthread_create(&event_thread, NULL, event_handler, NULL);

    pthread_join(monitoring_thread, NULL);
    pthread_join(comm_thread, NULL);
    pthread_join(event_thread, NULL);

    return 0;
}
