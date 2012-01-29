/**
 * Copyright (c) 2012 Björn Edström <be@bjrn.se>
 * All rights reserved.
 *
 * See LICENSE file for details.
 */

#include <stddef.h> // offsetof
#include <stdlib.h>
#include <unistd.h> // pathconf

#include "pidreader.h"

int open_pids(struct pid_reader *pr)
{
	size_t len;

	if ((pr->dir_proc = opendir("/proc")) == NULL) {
		return -1;
	}

	// See http://womble.decadent.org.uk/readdir_r-advisory.html.
	// However, since we are only interested in /proc we do not
	// pay much attention to this.
	len = offsetof(struct dirent, d_name) + pathconf("/proc", _PC_NAME_MAX) + 1;
	pr->entry = malloc(len);
	if (pr->entry == NULL) {
		return -1;
	}

	return 0;
}

// TODO (bjorn): I do not like this function and how it handles
// errors.
pid_t read_pids(struct pid_reader *pr)
{
	int maybe_pid;
	int ret;

	while ((ret = readdir_r(pr->dir_proc, pr->entry, &pr->entry_ptr)) == 0 && pr->entry_ptr) {
		if (pr->entry->d_type != DT_DIR) {
			continue;
		}

		maybe_pid = atoi(pr->entry->d_name);
		if (maybe_pid < 1)
			continue;

		return maybe_pid;
	}

	if (ret) {
		return -1;
	}

	return 0;
}

int reset_pids(struct pid_reader *pr)
{
	rewinddir(pr->dir_proc);
	return 0;
}

int close_pids(struct pid_reader *pr)
{
	free(pr->entry);
	if (closedir(pr->dir_proc) < 0) {
		return -1;
	}
	return 0;
}
