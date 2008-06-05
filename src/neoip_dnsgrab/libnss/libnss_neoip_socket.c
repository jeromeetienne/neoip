


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      forward request thru socket
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Send a string to the dest_addr and gather the responding string which is \n terminated
 * 
 * - it does nothing but socket stuff
 * - no parsing of the reply or building of the request_str
 * 
 * @return 0 if no *socket* error occured, -1 otherwise
 * 
 */
static int fwd_req_thru_socket(char *ipaddr_str, int port, int timeout_in_sec, char *request_str
					, char *reply_ptr, size_t reply_len )
{
	struct sockaddr_in	dest_addr;
	struct timeval		timeout;
	int	sockfd;
	int	err;
	char *	src_ptr	= request_str;
	ssize_t	src_len	= strlen(src_ptr)+1;
	char *	dst_ptr = reply_ptr;
	ssize_t	dst_len	= reply_len;

	// init the socket descriptor
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 )	return -1;

	// setup the destination address
	memset( &dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_addr.s_addr	= inet_addr(ipaddr_str);
	dest_addr.sin_port 		= htons(port);
	dest_addr.sin_family 		= AF_INET;

	// connect the socket
	err = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if( err < 0 )	goto error;

// send the request thru the socket
	do{
		// write data in the socket
		ssize_t	written_len = write(sockfd, src_ptr, src_len);
		if( written_len < 0 )	goto error;
		// update the pointer and len
		src_ptr += written_len;
		src_len -= written_len;
	}while( src_len > 0 );

// handle timeout via select
	// build the select set
	fd_set	sock_set;
	FD_ZERO(&sock_set);
	FD_SET(sockfd, &sock_set);
	timeout.tv_sec	= timeout_in_sec;
	timeout.tv_usec = 0;
	// do the select itself
	int has_data = select(sockfd+1, &sock_set, NULL, NULL, &timeout);
	// if it timeout, it is considered an error
	if( !has_data )	goto error;
	

// read the reply from the socket
	do{
		// read data from the socket
		ssize_t	read_len = read(sockfd, dst_ptr, dst_len);
		if( read_len < 0 || dst_len == 0 )	goto error;
		// if the remote peer closed the connection, exit the loop
		if( read_len == 0 )			break;
		// log to debug
		KLOG_DBG("read_len=%d\n", read_len);
		KLOG_DBG("reply_ptr=%s\n", reply_ptr);
		// update the pointer and len
		dst_ptr	+= read_len;
		dst_len -= read_len;
	}while( reply_ptr[reply_len-dst_len-1] == '\r' );
	// put a trailing \0	
	*dst_ptr = '\0';

	// close the socket descriptor
	close(sockfd);
	// return noerror
	return 0;
	
error:;	close(sockfd);
	return -1;
}


