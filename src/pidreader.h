/**
 * Copyright (c) 2012 Björn Edström <be@bjrn.se>
 * All rights reserved.
 *
 * See LICENSE file for details.
 */

#ifndef PIDREADER_H
#define PIDREADER_H

#include <sys/types.h>
#include <dirent.h>

struct pid_reader {
	DIR *dir_proc;
	struct dirent *entry;
	struct dirent *entry_ptr;
};

/**
 * Open a pid_reader, a snapshot of currently running pids. This is
 * implemented by reading pids from the /proc pseud-file system.
 */
extern int open_pids(struct pid_reader *pr);

/**
 * For each invocation returns one pid, or 0 when done.
 *
 * TODO (bjorn): Rewrite this to int read_pids(struct ..., pid_t *out)
 */
extern pid_t read_pids(struct pid_reader *pr);

/**
 * Reset the pid_reader so it's like completely new.
 */
extern int reset_pids(struct pid_reader *pr);

/**
 * Close the pid_reader.
 */
extern int close_pids(struct pid_reader *pr);

#endif /* PIDREADER_H */
