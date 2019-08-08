#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <dlfcn.h>
#include <unistd.h>

unsigned do_exit = 0;

void sigint_handler(int sig)
{
	(void) sig;
	printf("\rSIGINT received...\n");
	do_exit = 1;
}

void* open_lib(void)
{
#ifdef SAFE_DL_CALL
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)) {
		perror("pthread_setcancelstate() failed");
		return NULL;
	}
#endif

	void* handle = dlopen("libmpv.so", RTLD_NOW);
	if (handle == NULL) {
		fprintf(stderr, "dlopen() failed: %s\n", dlerror());
		return NULL;
	}

#ifdef SAFE_DL_CALL
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) {
		perror("pthread_setcancelstate() failed");
		return handle;
	}
#endif

	return handle;
}

void close_lib(void* handle)
{
	if (handle == NULL) {
		return;
	}

#ifdef SAFE_DL_CALL
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)) {
		perror("pthread_setcancelstate() failed");
		return;
	}
#endif

	if (dlclose(handle)) {
		fprintf(stderr, "close() failed: %s\n", dlerror());
	}

#ifdef SAFE_DL_CALL
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) {
		perror("pthread_setcancelstate() failed");
	}
#endif
}

void* thread_routine(void* data)
{
	(void) data;

#ifdef WITH_CANCEL_DEFERRED
	if (pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL)) {
		perror("pthread_setcanceltype() failed");
		return NULL;
	}
#endif

#ifdef WITH_CANCEL_ASYNC
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)) {
		perror("pthread_setcanceltype() failed");
		return NULL;
	}
#endif

	while(1) {
		void* handle = open_lib();

#ifdef WITH_CANCEL_POINT
		sleep(1);
#endif

		close_lib(handle);
	}
}

int main()
{
	pthread_t thread;

	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("signal() failed");
		exit(1);
	}

	printf("Starting threads...\n");

	while(!do_exit) {
		printf("Creating new thread... "); fflush(stdout);
		if (pthread_create(&thread, NULL, thread_routine, NULL)) {
			perror("pthread_create() failed");
			exit(1);
		}
		printf("done\n"); fflush(stdout);

		printf("Cancelling thread... "); fflush(stdout);
		if (pthread_cancel(thread)) {
			perror("pthread_cancel() failed");
		}
		printf("done\n"); fflush(stdout);

		printf("Joining thread... "); fflush(stdout);
		if (pthread_join(thread, NULL)) {
			perror("pthread_join() failed");
		}
		printf("done\n"); fflush(stdout);
	}

	printf("Exiting program...\n");

	return 0;
}
