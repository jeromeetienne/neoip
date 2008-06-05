/*! \file
    \brief Implementation of the remote iaddr negative cache

- used to return an ICMP if the ip record is in the ns_negcache
- a ip addr is setup in the negcache iff a ns_query or a itor timeout or return an error
  - the timeout apply the concept of not replying a icmp immediatly
    - similar to the time to solve the hw address with ARP
    - as in rfc2461.7.2.2, ICMP must be replied after 3sec

*/

/* system include */
#include <glib.h>
#include <string.h>

/* local include */
#include "dst_iaddr_negcache.h"
#include "util.h"
#include "httpd.h"

static void dst_iaddr_negcache_del( dst_iaddr_negcache_item_t *item  );

/**
 * function called when timeout expire
 */
static gboolean dst_iaddr_negcache_expire(gpointer userptr)
{
	dst_iaddr_negcache_item_t *item = userptr;
	dst_iaddr_negcache_del( item );
	return FALSE;
}

/**
 * return non-null if the question is in the cache
 * @param ttl if equal to 0, the default value is use
 */
int dst_iaddr_negcache_add( dst_iaddr_negcache_t *negcache, ip_addr_t *ip_addr, int ttl )
{
	// allocate the memory
	dst_iaddr_negcache_item_t	*item = nipmem_zalloc(sizeof(*item));
	// fill the struct
	item->ip_addr		= *ip_addr;
	item->negcache		= negcache;
	// if ttl == 0, the default value is used
	if( ttl == 0 )		ttl = DST_IADDR_NEGCACHE_TIMEOUT_SEC;
	// start the expiration timer
	item->timeout_tag	= g_timeout_add( ttl*1000, dst_iaddr_negcache_expire, item );
	// add it to the list
	negcache->neg_list	= g_list_append( negcache->neg_list, item );		
	return 0;
}

/**
 * return non-null if the question is in the cache
 */
static void dst_iaddr_negcache_del( dst_iaddr_negcache_item_t *item  )
{
	dst_iaddr_negcache_t	*negcache = item->negcache;
	// remove it from the list
	negcache->neg_list = g_list_remove( negcache->neg_list, item );		
	// stop the timer
	g_source_remove( item->timeout_tag );	
	// free the memory
	nipmem_free( item );
}

/**
 * return non-null if the question is in the cache
 */
int dst_iaddr_negcache_is_present( dst_iaddr_negcache_t *negcache, ip_addr_t *ip_addr )
{
	GList	*elem;
	for( elem = negcache->neg_list; elem; elem = g_list_next( elem ) ){
		dst_iaddr_negcache_item_t	*item = elem->data;
		if( ip_addr_is_equal(&item->ip_addr, ip_addr ) )
			return 1;
	}
	return 0;	
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// openlay/closelay function
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/**
 * open layer of dst_iaddr_negcache
 */
dst_iaddr_negcache_t *dst_iaddr_negcache_open(void)
{
	dst_iaddr_negcache_t *negcache = nipmem_zalloc(sizeof(*negcache));
	return negcache;
}

/**
 * close layer of dst_iaddr_negcache
 */
void dst_iaddr_negcache_close( dst_iaddr_negcache_t *negcache )
{
	GList	*elem;
	// free each item of the databadse
	while( (elem = negcache->neg_list) ){
		dst_iaddr_negcache_item_t	*item = elem->data;
		dst_iaddr_negcache_del( item );
	}
	// free memory
	nipmem_free( negcache );
}



