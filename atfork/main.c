#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <increment.c>

#define FORK_COUNT 3
#define INCREMENT_COUNT_HANDLER 100
#define INCREMENT_COUNT_MAIN 5000
#define CHILD_LIFETIME_US 1000

void sigint_handler(int signum)
{
	(void) signum;

	int child_count = 0;
	for (int i = 0; i < FORK_COUNT; i++) {
		int ret = fork();
		if (ret < 0) {
			perror("fork");
			continue;
		}

		if (ret == 0) {
			usleep(CHILD_LIFETIME_US);
			exit(0);
		} else {
			child_count++;
		}
	}

	for (int i = 0; i < child_count; i++) {
		if (wait(NULL) < 0) {
			perror("wait");
		}
	}
}

void before_fork(void)
{
	for (int i = 0; i < INCREMENT_COUNT_HANDLER; i++) {
		increment_counter();
	}
}

void after_fork_parent(void)
{
	for (int i = 0; i < INCREMENT_COUNT_HANDLER; i++) {
		increment_counter();
	}
}

int main()
{
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("signal");
		exit(1);
	}

	if (pthread_atfork(before_fork, after_fork_parent, NULL)) {
		perror("pthread_atfork");
		exit(1);
	}

	for (int i = 0; i < INCREMENT_COUNT_MAIN; i++) {
		increment_counter();
	}

	printf("\rcounter value: %d\n", counter);

	return 0;
}
