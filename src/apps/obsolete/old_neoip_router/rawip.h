/*! \file
    \brief Header of the Implementation of raw ip function

*/

#ifndef __RAWIP_H__
#define __RAWIP_H__
/* system include */
/* local include */
#include "otsp_addr.h"

/* prototype definition */
int raw_icmp_reply_send(int type, int code, int data, char *pkt, int pkti_len);
int raw_udp_send( otsp_addr_t *local_addr, otsp_addr_t *remote_addr, char *pkt, int pkt_len);

#endif	/* __RAWIP_H__ */

