#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#ifndef SLEEP_IN_INCREMENT
# define SLEEP_IN_INCREMENT 100
#endif

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void increment_counter()
{
#ifdef SIGNAL_SAFE
	sigset_t sigset_all_blocked, sigset_saved;

	if (sigfillset(&sigset_all_blocked) < 0) {
		perror("sigfillset");
	}

	if (pthread_sigmask(SIG_SETMASK, &sigset_all_blocked, &sigset_saved) < 0) {
		perror("pthread_sigmask");
	}
#endif

	if (pthread_mutex_lock(&mutex)) {
		perror("pthread_mutex_lock");
		return;
	}

	counter++;

	if (usleep(SLEEP_IN_INCREMENT) < 0) {
		perror("usleep");
	}

	if (pthread_mutex_unlock(&mutex)) {
		perror("pthread_mutex_unlock");
		return;
	}

#ifdef SIGNAL_SAFE
	if (pthread_sigmask(SIG_SETMASK, &sigset_saved, NULL) < 0) {
		perror("pthread_sigmask");
	}
#endif
}
