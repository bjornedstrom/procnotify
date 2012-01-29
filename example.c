/**
 * Copyright (c) 2012 Björn Edström <be@bjrn.se>
 * All rights reserved.
 *
 * See LICENSE file for details.
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <procnotify.h>

// Is this program still alive?
static int mainalive = 1;

void procnotify_status(pid_t pid, int status)
{
	char path[1024];
	char buf[1024];
	FILE *f;
	if (status == PROCNOTIFY_INIT)
		printf("init %d\n", pid);
	else if (status == PROCNOTIFY_CREAT) {
		sprintf(path, "/proc/%d/cmdline", pid);
		buf[0] = 0;
		f = fopen(path, "r");
		if (f) {
			fread(buf, 1, 1024, f);
			fclose(f);
		}
		printf("create %d [%s]\n", pid, buf);
	}
	else if (status == PROCNOTIFY_DESTROY)
		printf("destroy %d\n", pid);
	return;
}

void on_sigint(int n)
{
	procnotify_stop();
	mainalive = 0;
}

int main()
{
	signal(SIGINT, &on_sigint);

	procnotify_init(&procnotify_status, 100000);
	procnotify_start();

	while (mainalive) {
		printf("main thread\n");
		sleep(1);
	}

	return 0;
}
