typedef enum {
	NSS_ERR_PROTO		= -2,	//!< when there is an error in the protocol itself (like syntax
					//!< error in the incoming command or the socket being closed
					//!< unexpectedly, etc...)
	NSS_ERR_ERANGE		= -1,	//!< when the destination buffer is too small
	NSS_ERR_NOTFOUND	= +0,	//!< when the name is not found
	NSS_ERR_FOUND		= +1,	//!< when the name is found
} nss_err_t;

//! return true if the nss_err_t is fatal
#define NSS_ERR_IS_FATAL(nss_err)	((nss_err) == NSS_ERR_PROTO || (nss_err) == NSS_ERR_ERANGE)

/** \brief Convert the nss_err_t to the nss_status / errnop / h_errnop
 */
enum nss_status nss_err_to_nss_status(nss_err_t nss_err, int *errnop, int *h_errnop)
{
	// set the return varabls according to the local error
	switch( nss_err ){
	case NSS_ERR_FOUND:	*errnop		= 0;
				*h_errnop	= 0;
				return NSS_STATUS_SUCCESS;
	case NSS_ERR_NOTFOUND:	*errnop		= ENOENT;
				*h_errnop	= HOST_NOT_FOUND;
				return NSS_STATUS_TRYAGAIN;
	case NSS_ERR_PROTO:	*errnop		= ENOENT;
				*h_errnop	= HOST_NOT_FOUND;
				return NSS_STATUS_TRYAGAIN;
	case NSS_ERR_ERANGE:	*errnop		= ERANGE;
				*h_errnop	= NO_RECOVERY;
				return NSS_STATUS_TRYAGAIN;
	default:		assert( 0 );
	}
}

