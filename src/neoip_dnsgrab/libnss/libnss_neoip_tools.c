//! type to store an IPv4 address
typedef struct {
	uint32_t address;
} ipv4_addr_t;

//! type to store an IPv6 address
typedef struct {
	uint8_t	address[16];
} ipv6_addr_t;

#define KLOG_DBG	if( 0 ) printf
#define KLOG_ERR	if( 0 ) printf

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         my own strtok
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief My own strtok
 */
static char *my_strtok( char *base, char delim, char **state )
{
	char *	first = base ? base : *state;
	char *	cur;
	// handle the magic state to detect the end of the string
	// - if first == NULL, then it is because state has been set to null
	//   in the previous call, because the end of the string was reached.
	if( first == NULL )	return NULL;
	// scan the string until either delim or \0 is found
	for( cur = first; *cur != '\0' && *cur != delim; cur++ );
	
	// if the string is now fully parsed, set the state to NULL and return first
	if( *cur == '\0' ){
		*state	= NULL;
		return first;
	}

	// zero the char under cur
	*cur	= '\0';
	// set the state to the charater after cur
	*state	= cur+1;
	// return a pointer on the begining of this token
	return first;
}


