/*
 * Compile with:
 * cc -I/usr/local/include -o event-test event-test.c -L/usr/local/lib -levent
 */

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event.h>


int lasttime;


void
fifo_read(int fd, short event, void *arg)
{
	char buf[255];
	int len;
	struct event *ev = arg;

	/* Reschedule this event */
//	event_add(ev, NULL);

	fprintf(stderr, "fifo_read called with fd: %d, event: %d, arg: %p\n",
		fd, event, arg);

	len = read(fd, buf, sizeof(buf) - 1);

	if (len == -1) {
		perror("read");
		return;
	} else if (len == 0) {
		fprintf(stderr, "Connection closed\n");
		return;
	}

	buf[len] = '\0';

	fprintf(stdout, "Read: %s\n", buf);
}

void
timeout_cb(int fd, short event, void *arg)
{
	struct timeval tv;
	struct event *timeout = arg;
	int newtime = time(NULL);

	printf("%s: called at %d: %d\n", __func__, newtime,
	    newtime - lasttime);
	lasttime = newtime;

	timerclear(&tv);
	tv.tv_sec = 2;
	event_add(timeout, &tv);
}

int
main (int argc, char **argv)
{
	struct event	evfifo;
	struct event	timeout;
	struct timeval	tv;
	struct stat	st;
	char *		fifo	= "event.fifo";
	int		socket;
 
	if (lstat (fifo, &st) == 0) {
		if ((st.st_mode & S_IFMT) == S_IFREG) {
			errno = EEXIST;
			perror("lstat");
			exit (1);
		}
	}

	unlink (fifo);
	if (mkfifo (fifo, 0600) == -1) {
		perror("mkfifo");
		exit (1);
	}

	/* Linux pipes are broken, we need O_RDWR instead of O_RDONLY */
	socket = open (fifo, O_RDWR | O_NONBLOCK, 0);
	if (socket == -1) {
		perror("open");
		exit (1);
	}

	fprintf(stderr, "jme- Write data to %s\n", fifo);
	/* Initalize the event library */
	event_init();

	/* Initalize one event */
	event_set(&evfifo, socket, EV_READ | EV_PERSIST, fifo_read, &evfifo);

	/* Add it to the active events, without a timeout */
	event_add(&evfifo, NULL);

	lasttime = time(NULL);

	evtimer_set(&timeout, timeout_cb, &timeout);

	timerclear(&tv);
	tv.tv_sec = 2;
	event_add(&timeout, &tv);

	
	event_dispatch();

	return (0);
}

