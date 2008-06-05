/*! \file
    \brief header file of the tunnel negative cache
    
*/
#ifndef __DST_IADDR_NEGCACHE_H__
#define __DST_IADDR_NEGCACHE_H__

/* system include */
#include <glib.h>

/* local include */
#include "ip_addr.h"

typedef struct {
	ip_addr_t			ip_addr;
	guint				timeout_tag;
	struct dst_iaddr_negcache_stt 	*negcache;	//!< back pointer on the main struct
} dst_iaddr_negcache_item_t;

typedef struct dst_iaddr_negcache_stt {
	GList	*neg_list;		//!< list of record in the negative cache
} dst_iaddr_negcache_t;

#define DST_IADDR_NEGCACHE_TIMEOUT_SEC	(5)

/* prototype definition */
int dst_iaddr_negcache_add( dst_iaddr_negcache_t *negcache, ip_addr_t *ip_addr, int ttl );
int dst_iaddr_negcache_is_present( dst_iaddr_negcache_t *negcache, ip_addr_t *ip_addr );
dst_iaddr_negcache_t *dst_iaddr_negcache_open(void);
void dst_iaddr_negcache_close(dst_iaddr_negcache_t *negcache);

#endif	/* __DST_IADDR_NEGCACHE_H__ */
