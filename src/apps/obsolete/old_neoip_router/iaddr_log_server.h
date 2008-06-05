/*! \file
    \brief .h of the inner address on loggin server

*/
#ifndef __IADDR_LOG_SERVER_H__
#define __IADDR_LOG_SERVER_H__

/* system include */
/* local include */
#include "otsp_addr.h"
#include "ip_addr.h"
#include "nodeid_iaddr.h"

//! the list of state used by rdv_client
typedef enum {
	IADDR_LOG_SERVER_NONE,
	IADDR_LOG_SERVER_OK,
	IADDR_LOG_SERVER_NOT_AVAILABLE,
	IADDR_LOG_SERVER_MAX
} iaddr_log_server_result_t;


typedef struct {
	int			init_counter;		//!< initialisation counter
	ip_addr_t		next_inner_addr;
	ip_netmask_t		inner_netmask;
	nodeid_iaddr_db_t	nodeid_iaddr_db;
} iaddr_log_server_t;

/* prototype definition */
int iaddr_log_server_is_init( void );
int iaddr_log_server_open( void );
void iaddr_log_server_close( void );


#endif /* __IADDR_LOG_SERVER_H__ */


