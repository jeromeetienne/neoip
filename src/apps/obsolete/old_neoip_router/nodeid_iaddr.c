/*! \file
    \brief Implementation of the nodeid_iaddr database

*/

/* system include */
#include <time.h>
#include <string.h>
#include <stdio.h>

/* local include */
#include "nodeid_iaddr.h"
#include "util.h"
#include "nipmem.h"
#include "nipid.h"

static char *nodeid_iaddr_str( nodeid_iaddr_t *nodeid_iaddr )
{
	static	char	buf1[300], buf2[300];
	static 	int	var;
	char		*str	= var ? buf1 : buf2;
	var	^= 1;
	// put string in buffer
	snprintf( str, sizeof(buf1), "%s %s expire in %ld-sec"
						, nipid_str(&nodeid_iaddr->nodeid)
						, ip_addr_str(&nodeid_iaddr->ip_addr)
						, nodeid_iaddr->expiration_time - time(NULL));
	return str;
}
/**
 * disp the database
 */
void nodeid_iaddr_db_disp( nodeid_iaddr_db_t *db )
{
	GList	*elem;
	for( elem = db->nodeid_iaddr_list; elem; elem = g_list_next( elem ) ){
		nodeid_iaddr_t	*item = elem->data;
		DBG("%s\n", nodeid_iaddr_str(item) );
	}
}
/**
 * add an item to the database
 */
nodeid_iaddr_t *nodeid_iaddr_from_nodeid( nodeid_iaddr_db_t *db, nipid_t *nodeid )
{
	GList	*elem;
	for( elem = db->nodeid_iaddr_list; elem; elem = g_list_next( elem ) ){
		nodeid_iaddr_t	*item = elem->data;
		if( nipid_is_equal( &item->nodeid, nodeid ) )
			return item;
	}
	return NULL;
}
/**
 * add an item to the database
 */
nodeid_iaddr_t *nodeid_iaddr_from_ip_addr( nodeid_iaddr_db_t *db, ip_addr_t *ip_addr )
{
	GList	*elem;
	for( elem = db->nodeid_iaddr_list; elem; elem = g_list_next( elem ) ){
		nodeid_iaddr_t	*item = elem->data;
		if( ip_addr_is_equal( &item->ip_addr, ip_addr ) )
			return item;
	}
	return NULL;
}

/**
 * renew a nodeid_iaddr
 */
void nodeid_iaddr_renew( nodeid_iaddr_db_t *db, nodeid_iaddr_t *item )
{
	// remote it from the list
	db->nodeid_iaddr_list	= g_list_remove( db->nodeid_iaddr_list, item );
	// change the expiration time
	item->expiration_time	= time(NULL) + NODEID_IADDR_TTL_SEC;
	// add it to the list
	db->nodeid_iaddr_list	= g_list_append( db->nodeid_iaddr_list, item );
}

/**
 * add an item to the database
 */
nodeid_iaddr_t *nodeid_iaddr_db_add( nodeid_iaddr_db_t *db, nipid_t *nodeid, ip_addr_t *ip_addr )
{
	nodeid_iaddr_t	*item 	= nipmem_zalloc( sizeof(*item) );
	// fill the struct
	item->nodeid		= *nodeid;
	item->ip_addr		= *ip_addr;
	item->expiration_time	= time(NULL) + NODEID_IADDR_TTL_SEC;
	// add it to the list
	db->nodeid_iaddr_list = g_list_append( db->nodeid_iaddr_list, item );
	return item;
}

/**
 * delete an item to the database
 */
static void nodeid_iaddr_db_del( nodeid_iaddr_db_t *db, nodeid_iaddr_t *item )
{
	// remote it from the list
	db->nodeid_iaddr_list = g_list_remove( db->nodeid_iaddr_list, item );
	// free the memory
	nipmem_free( item );
}

/**
 * delete an item from its nodeid
 */
int nodeid_iaddr_db_del_from_nodeid( nodeid_iaddr_db_t *db, nipid_t *nodeid )
{
	nodeid_iaddr_t *item = nodeid_iaddr_from_nodeid( db, nodeid );
	if( !item )	return -1;
	nodeid_iaddr_db_del( db, item );
	return 0;
}

/**
 * remote from the db any item with a expired ttl
 * - assume the db is stored in increasing ttl
 *   o i.e. the first of the db is the lowest expiration time
 */
static void nodeid_iaddr_db_purge_expired( nodeid_iaddr_db_t *db )
{
	GList	*elem;
	time_t	current_time = time(NULL);
	DBG("try to purge expired nodeid_iaddr\n");
	while( (elem = db->nodeid_iaddr_list) ){
		nodeid_iaddr_t	*item = elem->data;
		if( item->expiration_time > current_time )
			break;
		nodeid_iaddr_db_del( db, item );
	}
}

/**
 * function called when a db->expire_timeout expire
 * - called periodacally to purge the database from its expired items
 */
static gboolean nodeid_iaddr_db_expire(gpointer userptr)
{
	nodeid_iaddr_db_t	*db = userptr;
	nodeid_iaddr_db_purge_expired( db );
	return TRUE;
}

/**
 * open a nodeid_iaddr database
 */
int nodeid_iaddr_db_open( nodeid_iaddr_db_t *db )
{
	// zero the struct
	memset( db, 0, sizeof(*db) );
	// start the expiration timer
	db->expire_timeout = g_timeout_add( NODEID_IADDR_DB_EXPIRE_TIMEOUT
					, nodeid_iaddr_db_expire, db );	
	return 0;
}

/**
 * close a nodeid_iaddr database
 */
void nodeid_iaddr_db_close( nodeid_iaddr_db_t *db )
{
	GList	*elem;
	// stop the expiration timer
	g_source_remove( db->expire_timeout );	
	// free each item of the databadse
	while( (elem = db->nodeid_iaddr_list) ){
		nodeid_iaddr_t	*item = elem->data;
		nodeid_iaddr_db_del( db, item );
	}
}


/**
 * load a database from a file
 */
int nodeid_iaddr_db_load( nodeid_iaddr_db_t *db, char *fname )
{
	FILE	*fIn;
	struct {
		nipid_t		nodeid;
		payl_ip_addr_t	payl_ip_addr;
		uint32_t	expiration_time;
	} loaded_item;
	// open the file 
	fIn = fopen( fname, "r" );
	if( fIn == NULL )	return -1;
	// load each item
	while( fread( &loaded_item, sizeof(loaded_item), 1, fIn ) == 1 ){
		nodeid_iaddr_t	*item 	= nipmem_zalloc( sizeof(*item) );
		// fill the struct
		item->nodeid		= loaded_item.nodeid;
		ip_addr_from_payload( &item->ip_addr, &loaded_item.payl_ip_addr );
		item->expiration_time	= ntohl(loaded_item.expiration_time);
		// add it to the list
		db->nodeid_iaddr_list	= g_list_append( db->nodeid_iaddr_list, item );			
	}
	// close the file
	fclose( fIn );
	// purge expired nodeid_iaddr
	nodeid_iaddr_db_purge_expired( db );
	// loggin for debug
	DBG("LOADED: \n"); nodeid_iaddr_db_disp( db );
	return 0;
}

/**
 * save a database to a file
 * - TODO error case are very badly handled
 * - if the disk is full the database is fully corrupted
 *   o solution, store it elsewhere and move it to the first fname once everything worked
 */
int nodeid_iaddr_db_save( nodeid_iaddr_db_t *db, char *fname )
{
	FILE	*fOut;
	GList	*elem;
	payl_ip_addr_t	payl_ip_addr;
	DBG("SAVE: \n"); nodeid_iaddr_db_disp( db );
	// open the file 
	fOut = fopen( fname, "w+" );
	if( fOut == NULL )	return -1;
	// sanity check
	DBG_ASSERT( sizeof(uint32_t) == sizeof(time_t) );
	// save each item
	for( elem = db->nodeid_iaddr_list; elem; elem = g_list_next( elem ) ){
		nodeid_iaddr_t	*item = elem->data;
		uint32_t	tmp_time;
		// save the nodeid
		if( fwrite( &item->nodeid, sizeof(item->nodeid), 1, fOut ) != 1 )	goto error;
		// save the ip addr
		ip_addr_to_payload( &payl_ip_addr, &item->ip_addr );
		if( fwrite( &payl_ip_addr, sizeof(payl_ip_addr_t), 1, fOut ) != 1 )	goto error;
		// save creation time
		tmp_time = htonl(item->expiration_time);
		if( fwrite( &tmp_time, sizeof(tmp_time), 1, fOut ) != 1 )		goto error;
	}
	// close the file
	fclose( fOut );
	return 0;
error:;	LOGM_ERR("cant save nodeid_iaddr_db to %s\n", fname );
	fclose( fOut );
	return -1;
}



