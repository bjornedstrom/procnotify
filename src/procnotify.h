/**
 * Copyright (c) 2012 Björn Edström <be@bjrn.se>
 * All rights reserved.
 *
 * See LICENSE file for details.
 */

#ifndef PROCNOTIFY_H
#define PROCNOTIFY_H

#include <sys/types.h>

#define PROCNOTIFY_INIT 0
#define PROCNOTIFY_CREAT 1
#define PROCNOTIFY_DESTROY 2

typedef void (*procnotify_func)(pid_t, int);

/**
 *
 */
extern int procnotify_init(procnotify_func func);

/**
 *
 */
extern int procnotify_start();

/**
 *
 */
extern int procnotify_stop();

#endif /* PROCNOTIFY_H */
