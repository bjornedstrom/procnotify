/**
 * Copyright (c) 2012 Björn Edström <be@bjrn.se>
 * All rights reserved.
 *
 * See LICENSE file for details.
 */

#ifndef PROCNOTIFY_H
#define PROCNOTIFY_H

#include <sys/types.h>
#include <unistd.h>

#define PROCNOTIFY_INIT 0
#define PROCNOTIFY_CREAT 1
#define PROCNOTIFY_DESTROY 2

/**
 * Callback function for notifications.
 * @pid_t: The pid of the process.
 *
 * @int: Status, one of the PROCNOTIFY_ constants. The constants have
 * the following meanings:
 * PROCNOTIFY_INIT is sent immediately on procnotify_start() for
 * pids that are already running when procnotify_start() is
 * called.
 * PROCNOTIFY_CREAT is sent when a pid has been created after
 * procnotify_start() was called.
 * PROCNOTIFY_DESTROY is similar to PROCNOTIFY_CREAT but for
 * process destruction.
 */
typedef void (*procnotify_func)(pid_t, int);

/**
 * Initialize the procnotify library. Should only be called once.
 *
 * @func: A callback function to receive notications on process
 * creation/destruction. Note that this callback is invoked from the
 * procnotify background thread. It is up to the user of the library
 * to handle synchronization with the main program.
 *
 * @poll_interval: useconds between when processes are polled.
 */
extern int procnotify_init(procnotify_func func, useconds_t poll_interval);

/**
 * Start the procnotify background thread. The procnotify_func given
 * in procnotify_init will immediately start to get notifications.
 */
extern int procnotify_start();

/**
 * Stops the procnotify background thread. This may block for up to
 * poll_interval useconds.
 */
extern int procnotify_stop();

#endif /* PROCNOTIFY_H */
