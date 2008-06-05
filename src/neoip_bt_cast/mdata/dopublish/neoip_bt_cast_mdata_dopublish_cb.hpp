/*! \file
    \brief Declaration of the bt_cast_mdata_dopublish_t
    
*/


#ifndef __NEOIP_BT_CAST_MDATA_DOPUBLISH_CB_HPP__ 
#define __NEOIP_BT_CAST_MDATA_DOPUBLISH_CB_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_cast_mdata_dopublish_t;
class	bt_cast_mdata_t;

/** \brief the callback class for bt_cast_mdata_dopublish_t
 */
class bt_cast_mdata_dopublish_cb_t {
public:
	/** \brief callback notified by \ref bt_cast_mdata_dopublish_t
	 */
	virtual bool neoip_bt_cast_mdata_dopublish_cb(void *cb_userptr
					, bt_cast_mdata_dopublish_t &cb_mdata_dopublish
					, bt_cast_mdata_t *cast_mdata_out
					, uint16_t *port_lview_out, uint16_t *post_pview_out
					, std::string *casti_uri_pathquery)	throw() = 0;
	// virtual destructor
	virtual ~bt_cast_mdata_dopublish_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_MDATA_DOPUBLISH_CB_HPP__  */



