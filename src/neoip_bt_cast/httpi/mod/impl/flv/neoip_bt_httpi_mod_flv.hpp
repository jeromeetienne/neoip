/*! \file
    \brief Header of the bt_httpi_mod_flv_t
    
*/


#ifndef __NEOIP_BT_HTTPI_MOD_FLV_HPP__ 
#define __NEOIP_BT_HTTPI_MOD_FLV_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_httpi_mod_flv.hpp"
#include "neoip_bt_httpi_mod_vapi.hpp"
#include "neoip_bt_httpi_mod_type.hpp"
#include "neoip_bt_cast_spos_arr.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_datum.hpp"
#include "neoip_flv_parse_cb.hpp"
#include "neoip_flv_tophd.hpp"
#include "neoip_flv_taghd.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_httpi_t;
class	flv_parse_t;

/** \brief Handle the module 'flv' to deliver flv
 */
class bt_httpi_mod_flv_t : public bt_httpi_mod_vapi_t, NEOIP_COPY_CTOR_DENY
					, private flv_parse_cb_t {
private:
	bt_httpi_t *		m_bt_httpi;	//!< backpointer on the bt_httpi_t
	bt_httpi_mod_type_t	m_type;		//!< the type of this bt_httpi_mod_vapi_t
	bt_cast_spos_arr_t	m_cast_spos_arr;//!< the queued cast_spos database
	
	/*************** stuff for prefix_header	***********************/
	flv_tophd_t		m_flv_tophd;	//!< the flv_tophd_t received from the stream
	flv_taghd_t		m_meta_taghd;	//!< first flv_taghd_t from stream
	datum_t			m_meta_tagdata;	//!< flv_tagtype_t::META data from stream
	
	/*************** flv_parse_t	***************************************/
	flv_parse_t *	flv_parse;	
	bool		neoip_flv_parse_cb(void *cb_userptr, flv_parse_t &cb_flv_parse
					, const flv_parse_event_t &parse_event)		throw();
	bool		parse_for_cast_spos(const flv_parse_event_t &parse_event)	throw();
	bool		parse_for_prefix_header(const flv_parse_event_t &parse_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_httpi_mod_flv_t() 		throw();
	~bt_httpi_mod_flv_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(bt_httpi_t *m_bt_httpi)		throw();

	/*************** bt_httpi_mod_vapi_t	*******************************/
	const bt_httpi_mod_type_t & type()	const throw()	{ return m_type;	}
	void		notify_data(const datum_t &data)	throw();
	bt_cast_spos_t	cast_spos_pop()				throw();
	datum_t		prefix_header()				const throw();

	/*************** List of friend class	*******************************/
	friend class	bt_httpi_mod_flv_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPI_MOD_FLV_HPP__  */



