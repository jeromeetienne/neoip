/*! \file
    \brief Declaration of the bt_cast_mdata_server_t
    
*/


#ifndef __NEOIP_BT_CAST_MDATA_SERVER_CB_HPP__ 
#define __NEOIP_BT_CAST_MDATA_SERVER_CB_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_cast_mdata_server_t;
class	bt_cast_mdata_t;
class	bt_cast_id_t;

/** \brief the callback class for bt_cast_mdata_server_t
 */
class bt_cast_mdata_server_cb_t {
public:
	/** \brief callback notified by \ref bt_cast_mdata_server_t
	 */
	virtual bool neoip_bt_cast_mdata_server_cb(void *cb_userptr
					, bt_cast_mdata_server_t &cb_mdata_server
					, const bt_cast_id_t &cast_id
					, bt_cast_mdata_t *cast_mdata_out)	throw() = 0;
	// virtual destructor
	virtual ~bt_cast_mdata_server_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_MDATA_SERVER_CB_HPP__  */



