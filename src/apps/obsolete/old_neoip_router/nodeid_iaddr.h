/*! \file
    \brief header of Implementation of connection initiator

*/

#ifndef __NODEID_IADDR_H__
#define __NODEID_IADDR_H__
/* system include */
#include <glib.h>

/* local include */
#include "ip_addr.h"
#include "nipid_t.h"

typedef struct {
	nipid_t		nodeid;
	ip_addr_t	ip_addr;
	uint32_t	expiration_time;
} nodeid_iaddr_t;

typedef struct {
	GList		*nodeid_iaddr_list;
	// timeout for the expiration
	guint		expire_timeout;
} nodeid_iaddr_db_t;

#define	NODEID_IADDR_DB_EXPIRE_TIMEOUT	(10*1000)	//!< the frequency of database purge
#define NODEID_IADDR_TTL_SEC		(24*60*60)	//!< ttl of a given address allocated to a given nodeid

/* prototype definition */
nodeid_iaddr_t *nodeid_iaddr_from_nodeid( nodeid_iaddr_db_t *db, nipid_t *nodeid );
nodeid_iaddr_t *nodeid_iaddr_from_ip_addr( nodeid_iaddr_db_t *db, ip_addr_t *ip_addr );
void nodeid_iaddr_renew( nodeid_iaddr_db_t *db, nodeid_iaddr_t *item );
nodeid_iaddr_t *nodeid_iaddr_db_add( nodeid_iaddr_db_t *db, nipid_t *nodeid, ip_addr_t *ip_addr );
int nodeid_iaddr_db_open( nodeid_iaddr_db_t *db );
void nodeid_iaddr_db_close( nodeid_iaddr_db_t *db );
int nodeid_iaddr_db_load( nodeid_iaddr_db_t *db, char *fname );
int nodeid_iaddr_db_save( nodeid_iaddr_db_t *db, char *fname );


#endif	/* __NODEID_IADDR_H__ */

