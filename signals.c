#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "increment.c"

#define INCREMENTS 5000

void sigint_handler(int signum)
{
	(void) signum;

	increment_counter();
}

int main()
{
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("signal");
		exit(1);
	}

	for (int i = 0; i < INCREMENTS; i++) {
		increment_counter();
	}
	
	printf("\rcounter value: %d\n", counter);

	return 0;
}
