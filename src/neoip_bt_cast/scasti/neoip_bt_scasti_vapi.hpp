/*! \file
    \brief Header of the \ref bt_scasti_vapi_t
*/


#ifndef __NEOIP_BT_SCASTI_VAPI_HPP__
#define __NEOIP_BT_SCASTI_VAPI_HPP__

/* system include */
/* local include */
#include "neoip_file_size.hpp"
#include "neoip_bt_scasti_mod_vapi.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_uri_t;
class	bt_io_vapi_t;
class	bt_scasti_event_t;
class	bt_scasti_cb_t;

/** \brief Definition of the virtual API for the bt_scasti_t implementations
 */
class bt_scasti_vapi_t {
public:
	/*************** query function	***************************************/
	virtual const file_size_t &	cur_offset()	const throw()	= 0;
	virtual bt_scasti_mod_vapi_t *	mod_vapi()	const throw()	= 0;

	virtual	bool	mod_vapi_notify_callback(const bt_scasti_event_t &event)	throw() = 0;

	//! virtual destructor
	virtual ~bt_scasti_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SCASTI_VAPI_HPP__  */



