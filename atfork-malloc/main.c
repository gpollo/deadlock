#define _BSD_SOURCE
#define _DEFAULT_SOURCE


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <gnu/libc-version.h>

#define THREAD_COUNT 3

static int exits[2] = {0, 0};

static pthread_t prepare_threads[THREAD_COUNT];
static pthread_t parent_threads[THREAD_COUNT];

static
void useless_alloc(void)
{
	void* ptr = malloc(16);
	if (ptr == NULL) {
		perror("malloc");
	}

	free(ptr);
}

static
void bunch_of_useless_alloc(void)
{
	for (int i = 0; i < 50; i++) {
		useless_alloc();
	}
}

static
void* thread_routine(void* data)
{
	int* exit_thread = (int*) data;

	while (!exit_thread[0]) {
		useless_alloc();
	}

	return NULL;
}

static
int create_threads(int id)
{
	exits[0] = 0;
	if (pthread_create(&prepare_threads[id], NULL, thread_routine, &exits[0])) {
		perror("pthread_create");
		return -1;
	}

	exits[1] = 0;
	if (pthread_create(&parent_threads[id], NULL, thread_routine, &exits[1])) {
		perror("pthread_create");
		return -1;
	}

	return 0;
}

static
int create_all_threads(void)
{
	for (int i = 0; i < THREAD_COUNT; i++) {
		if (create_threads(i) < 0) {
			return -1;
		}
	}

	return 0;
}

static
int join_prepare_threads(void)
{
	for (int i = 0; i < THREAD_COUNT; i++) {
		useless_alloc();
		if (pthread_join(prepare_threads[i], NULL)) {
			perror("pthread_join");
			return -1;
		}
	}

	return 0;
}

static
int join_parent_threads(void)
{
	for (int i = 0; i < THREAD_COUNT; i++) {
		useless_alloc();
		if (pthread_join(parent_threads[i], NULL)) {
			perror("pthread_join");
			return -1;
		}
	}

	return 0;
}

#ifndef JOIN_IN_HANDLER
static
int join_all_threads(void)
{
	if (join_prepare_threads() < 0) {
		return -1;
	}

	if (join_parent_threads() < 0) {
		return -1;
	}

	return 0;
}
#endif

static
void atfork_prepare(void)
{
	bunch_of_useless_alloc();
	exits[0] = 1;
	bunch_of_useless_alloc();
#ifdef JOIN_IN_HANDLER
	(void) join_prepare_threads();
	bunch_of_useless_alloc();
#endif
}

static
void atfork_parent(void)
{
	bunch_of_useless_alloc();
	exits[1] = 1;
	bunch_of_useless_alloc();
#ifdef JOIN_IN_HANDLER
	(void) join_parent_threads();
	bunch_of_useless_alloc();
#endif
}

static 
int do_fork(void)
{
	printf("forking... ");
	if (fflush(stdout)) {
		perror("fflush");
		return -1;
	}

	if (create_all_threads() < 0) {
		return -1;
	}

	if (usleep(100000) < 0) {
		perror("usleep");
		return -1;
	}

	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		return -1;
	}

	if (child != 0) {
		if (wait(NULL) < 0) {
			perror("wait");
			return -1;
		}
	} else {
		_exit(0);
	}

#ifndef JOIN_IN_HANDLER
	if (join_all_threads() < 0) {
		return -1;
	}
#endif
	
	printf("done\n");

	return 0;
}

int main()
{
	printf("Using glibc %s version %s\n",
		gnu_get_libc_release(),
		gnu_get_libc_version()
	);

	errno = pthread_atfork(atfork_prepare, atfork_parent, NULL);
	if (errno) {
		perror("pthread_atfork");
		return -1;
	}

	for (int i = 0; i < 100; i++) {
		if (do_fork() < 0) {
			return -1;
		}
	}

	return 0;
}
