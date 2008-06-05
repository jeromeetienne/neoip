/** \brief derived from the ex-client2 of the gnutls example
 */


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

/* A very basic TLS client, with X.509 authentication.
 */

#define MAX_BUF 1024
#define CAFILE "ca.pem"
#define MSG "GET / HTTP/1.0\r\n\r\n"

extern int tcp_connect(void);
extern void tcp_close(int sd);
extern ssize_t tcp_write(gnutls_transport_ptr_t sock_fd,
			 const void *buf_ptr, size_t buf_len);
extern ssize_t tcp_read(gnutls_transport_ptr_t sock_fd, void *buf_ptr,
			size_t buf_len);
extern void tcp_nonblock(int sockfd);
extern int print_info(gnutls_session_t session);

/** \brief Custom log function
 */
static void tls_log_func(int level, const char *str)
{
	fprintf(stderr, "|<%d>| %s", level, str);
}

int main(void)
{
	int ret, sd, ii;
	gnutls_session_t session;
	char buffer[MAX_BUF + 1];
	gnutls_certificate_credentials_t xcred;
	// Allow connections to servers that have X509
	const int cert_type_priority[] = { GNUTLS_CRT_X509, 0 };

	gnutls_global_init();

	gnutls_global_set_log_function(tls_log_func);
	gnutls_global_set_log_level(2);

	// X509 stuff
	gnutls_certificate_allocate_credentials(&xcred);

	// sets the trusted cas file
//	gnutls_certificate_set_x509_trust_file(xcred, CAFILE,
//					       GNUTLS_X509_FMT_PEM);

	// Initialize TLS session 
	gnutls_init(&session, GNUTLS_CLIENT);

	// Use default priorities
	gnutls_set_default_priority(session);
	gnutls_certificate_type_set_priority(session, cert_type_priority);

	// put the x509 credentials to the current session
	gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, xcred);

	// connect to the peer
	sd = tcp_connect();

	// pass the socket descriptor in non blocking
	tcp_nonblock(sd);
	// set all the custom read/write function
	gnutls_transport_set_ptr(session, (gnutls_transport_ptr_t) sd);
	gnutls_transport_set_pull_function(session, tcp_read);
	gnutls_transport_set_push_function(session, tcp_write);

	// Perform the TLS handshake - until completed or error
	do {
		ret = gnutls_handshake(session);
		if(ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED)
			usleep(1000 * 10);
	}
	while(ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED);

	if(ret < 0) {
		fprintf(stderr, "*** Handshake failed\n");
		goto end;
	} else {
		printf("- Handshake was completed\n");
	}
	
	// log to debug
	print_info(session);

	ssize_t		written_len;
	written_len	= gnutls_record_send(session, MSG, strlen(MSG));
	printf("written_len=%d\n", written_len);

	do{
		ret = gnutls_record_recv(session, buffer, MAX_BUF);
		if(ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED)
			usleep(1000 * 10);
	}while(ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED);

	if(ret == 0) {
		printf("- Peer has closed the TLS connection\n");
		goto end;
	} else if(ret < 0) {
		fprintf(stderr, "*** Error: %s\n", gnutls_strerror(ret));
		goto end;
	}

	printf("- Received %d bytes: ", ret);
	for(ii = 0; ii < ret; ii++) {
		fputc(buffer[ii], stdout);
	}
	fputs("\n", stdout);

	gnutls_bye(session, GNUTLS_SHUT_RDWR);

      end:
	tcp_close(sd);
	gnutls_deinit(session);
	gnutls_certificate_free_credentials(xcred);
	gnutls_global_deinit();
	return 0;
}
