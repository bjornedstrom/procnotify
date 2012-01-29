/**
 * Copyright (c) 2012 Björn Edström <be@bjrn.se>
 * All rights reserved.
 *
 * See LICENSE file for details.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pidreader.h"
#include "procnotify.h"

static useconds_t g_alive = 1;
static void (*g_func)(pid_t, int);
static int g_sleep = 100000;
static pthread_t g_thr;

struct thread_data {
};

time_t pid_ctime(pid_t pid)
{
	char path[256];
	struct stat stat_buf;
	sprintf(path, "/proc/%d", pid);
	if (stat(path, &stat_buf) < 0) {
		return -1;
	}
	return stat_buf.st_ctime;
}

struct pid_info {
	pid_t pid;
	time_t ctime;
	char mark;
};

struct pid_state {
	size_t num_entries;
	struct pid_info *entries;
};

struct pid_state* init_pid_state()
{
	struct pid_state *s;
	s = malloc(sizeof(struct pid_state));
	s->num_entries = 0;
	s->entries = malloc(sizeof(struct pid_info));
	return s;
}

int add_pid_state(struct pid_state *s, struct pid_info *info)
{
	s->entries[s->num_entries].pid = info->pid;
	s->entries[s->num_entries].ctime = info->ctime;
	s->entries[s->num_entries].mark = 0;
	s->num_entries += 1;

	s->entries = realloc(s->entries, (s->num_entries + 1) * sizeof(struct pid_info));

	return 0;
}

int free_pid_state(struct pid_state *s)
{
	s->num_entries = 0;
	free(s->entries);
	free(s);
	return 0;
}

int state_diff(struct pid_state *cur, struct pid_state *prev)
{
	size_t c = 0, p = 0;
	struct pid_info *info_cur, *info_prev;

	while ((c < cur->num_entries) && (p < prev->num_entries)) {
		info_cur = &cur->entries[c];
		info_prev = &prev->entries[p];

		if (info_cur->pid == info_prev->pid) {
			if (info_cur->ctime != info_prev->ctime) {
				// the weird state
				g_func(info_cur->pid, PROCNOTIFY_DESTROY);
				g_func(info_cur->pid, PROCNOTIFY_CREAT);
			}
			c++;
			p++;
		} else if (info_cur->pid < info_prev->pid) {
			g_func(info_cur->pid, PROCNOTIFY_CREAT);
			c++;
		} else {
			g_func(info_prev->pid, PROCNOTIFY_DESTROY);
			p++;
		}
	}

	while (c < cur->num_entries) {
		info_cur = &cur->entries[c];
		g_func(info_cur->pid, PROCNOTIFY_CREAT);
	}

	while (p < prev->num_entries) {
		info_prev = &prev->entries[p];
		g_func(info_prev->pid, PROCNOTIFY_DESTROY);
	}

	return 0;
}


int procnotify_init(procnotify_func func, useconds_t poll_interval)
{
	g_alive = 1;
	g_func = func;

	// 0.1 seconds
	g_sleep = poll_interval;
	return 0;
}

void *poller_thread(void *arg)
{
	//struct thread_data *data = (struct thread_data *)arg;

	struct pid_state *state_cur;
	struct pid_state *state_prev;

	struct pid_info info;
	struct pid_reader pr;
	pid_t pid;

	if (open_pids(&pr) < 0) {
		// TODO: handle error
		return NULL;
	}

	state_cur = init_pid_state();
	state_prev = init_pid_state();

	while ((pid = read_pids(&pr))) {
		time_t ctime = pid_ctime(pid);

		info.pid = pid;
		info.ctime = ctime;

		g_func(pid, PROCNOTIFY_INIT);

		add_pid_state(state_cur, &info);
		add_pid_state(state_prev, &info);
	}

	while (g_alive) {
		free_pid_state(state_prev);
		state_prev = state_cur;
		state_cur = init_pid_state();

		reset_pids(&pr);
		while ((pid = read_pids(&pr))) {
			time_t ctime = pid_ctime(pid);
			info.pid = pid;
			info.ctime = ctime;
			add_pid_state(state_cur, &info);
		}

		state_diff(state_cur, state_prev);

		usleep(g_sleep);
	}

	if (close_pids(&pr) < 0) {
		// TODO: handle error
	}

	pthread_exit(NULL);
}

int procnotify_start()
{
	struct thread_data data;
	if (pthread_create(&g_thr, NULL, poller_thread, &data) != 0) {
		return -1;
	}
	return 0;
}

int procnotify_stop()
{
	g_alive = 0;
	pthread_join(g_thr, NULL);
	return 0;
}
