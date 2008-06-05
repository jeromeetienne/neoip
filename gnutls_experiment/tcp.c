#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <gnutls/gnutls.h>
#include <fcntl.h>

#define SA struct sockaddr

/* Connects to the peer and returns a socket
 * descriptor.
 */
extern int tcp_connect(void)
{
#if 1
	const char *PORT = "443";
	const char *SERVER = "192.168.1.3";
#else
	const char *PORT = "5556";
	const char *SERVER = "127.0.0.1";
#endif
	int err, sd;
	struct sockaddr_in sa;

	/* connects to server
	 */
	sd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&sa, '\0', sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(atoi(PORT));
	inet_pton(AF_INET, SERVER, &sa.sin_addr);

	err = connect(sd, (SA *) & sa, sizeof(sa));
	if(err < 0) {
		fprintf(stderr, "Connect error\n");
		exit(1);
	}

	return sd;
}

/* closes the given socket descriptor.
 */
extern void tcp_close(int sd)
{
	shutdown(sd, SHUT_RDWR);	/* no more receptions */
	close(sd);
}

ssize_t
tcp_write(gnutls_transport_ptr_t sock_fd, const void *buf_ptr,
	  size_t buf_len)
{
	printf("tcp_write len=%d\n", buf_len);
	return write((int) sock_fd, buf_ptr, buf_len);
}

ssize_t
tcp_read(gnutls_transport_ptr_t sock_fd, void *buf_ptr, size_t buf_len)
{
	printf("tcp_read len=%d\n", buf_len);
	return recv((int) sock_fd, buf_ptr, buf_len, 0);
}

void tcp_nonblock(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}
