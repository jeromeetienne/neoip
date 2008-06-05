/*===========================[ (c) JME SOFT ]===================================
FILE        : [onet.h]
CREATED     : 01/03/27 23:31:17		LAST SAVE    : 01/11/16 08:14:20
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================

==============================================================================*/

#ifndef __ONET_H__
#define __ONET_H__
/* system include */
/* local include */
#include "vdev.h"
#include "ip_addr.h"

#define ONET_TUNNEL_IDLE_TIMEOUT	(3*60*1000)	//!< if no data is exchanged on this tunnel for that long, close it (if <= 0, means never)
#define ONET_DNS_RECORD_TTL		(30*60)		//!< query ttl for the dns:hostname and dns:ip_addr record
							//! no need for it to be short as the content of those record
							//! is very stable
#define ONET_DELAY_B4_ICMP		(20*1000)	//!< nb ms before the connection to a destination iaddr is assumed unreachable, and icmp start to be replied

/* prototype definition */
int onet_openlay( void );
void onet_closelay( void );
int onet_vdev_xmit( int ethertype, char *pkt, int pkt_len );
int onet_is_local_addr( ip_addr_t *ip_addr );

#endif /* __ONET_H__ */
