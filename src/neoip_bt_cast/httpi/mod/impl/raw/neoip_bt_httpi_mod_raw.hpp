/*! \file
    \brief Header of the bt_httpi_mod_raw_t
    
*/


#ifndef __NEOIP_BT_HTTPI_MOD_RAW_HPP__ 
#define __NEOIP_BT_HTTPI_MOD_RAW_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_httpi_mod_raw.hpp"
#include "neoip_bt_httpi_mod_raw_profile.hpp"
#include "neoip_bt_httpi_mod_vapi.hpp"
#include "neoip_bt_httpi_mod_type.hpp"
#include "neoip_bt_cast_spos_arr.hpp"
#include "neoip_bt_cast_spos.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_timeout.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_httpi_t;

/** \brief Handle the module 'raw' to deliver raw
 */
class bt_httpi_mod_raw_t : public bt_httpi_mod_vapi_t, NEOIP_COPY_CTOR_DENY
			, private timeout_cb_t, private zerotimer_cb_t {
private:
	bt_httpi_t *			m_bt_httpi;	//!< backpointer on the bt_httpi_t
	bt_httpi_mod_raw_profile_t	m_profile;
	bt_httpi_mod_type_t		m_type;		//!< the type of this bt_httpi_mod_vapi_t
	bt_cast_spos_arr_t		m_cast_spos_arr;//!< the queued cast_spos database
	file_size_t			m_last_spos_offset;

	/*************** Internal function	*******************************/
	void		cast_spos_queue(const file_size_t &byte_offset)		throw();

	/*************** zerotimer	***************************************/
	zerotimer_t	event_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** spos_timeout	***************************************/
	timeout_t	spos_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_httpi_mod_raw_t() 		throw();
	~bt_httpi_mod_raw_t()		throw();

	/*************** Setup function	***************************************/
	bt_httpi_mod_raw_t &	profile(const bt_httpi_mod_raw_profile_t &p_profile)	throw();
	bt_err_t		start(bt_httpi_t *p_bt_httpi)							throw();

	/*************** query function	***************************************/
	bt_httpi_t *		bt_httpi()	const throw()	{ return m_bt_httpi;	}

	/*************** bt_httpi_mod_vapi_t	*******************************/
	const bt_httpi_mod_type_t & type()	const throw()	{ return m_type;	}
	void		notify_data(const datum_t &data)	throw();
	bt_cast_spos_t 	cast_spos_pop()				throw();
	datum_t		prefix_header()		const throw()	{ return datum_t();	}

	/*************** List of friend class	*******************************/
	friend class	bt_httpi_mod_raw_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPI_MOD_RAW_HPP__  */



