/*! \file
    \brief Class to handle the bt_scasti_mod_raw_t

\par Brief Description
bt_scasti_mod_raw_t is a bt_scasti_mod_vapi_t which doesnt modify the stream or provide
any kind of data. this is just a place holder for bt_scasti_mod_vapi_t.

*/

/* system include */
/* local include */
#include "neoip_bt_scasti_mod_raw.hpp"
#include "neoip_bt_scasti_vapi.hpp"
#include "neoip_bt_scasti_event.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_scasti_mod_raw_t::bt_scasti_mod_raw_t()	throw()
{
	// zero some variable
	m_type	= bt_scasti_mod_type_t::RAW;
}

/** \brief Destructor
 */
bt_scasti_mod_raw_t::~bt_scasti_mod_raw_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_scasti_mod_raw_t &	bt_scasti_mod_raw_t::profile(const bt_scasti_mod_raw_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_scasti_mod_raw_t::start(bt_scasti_vapi_t *p_scasti_vapi)	throw()
{
	// copy the parameters
	this->m_scasti_vapi	= p_scasti_vapi;

	// put a bt_cast_spos_t
	m_last_spos_offset	= 0;
	// put a byte_offset of 0 before starting
	m_cast_spos_arr.append( bt_cast_spos_t(m_last_spos_offset, date_t::present()) );

	// start the spos_timeout
	spos_timeout.start(m_profile.spos_maxdelay(), this, NULL );

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_scasti_mod_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify new data to the this bt_scasti_mod_vapi_t
 */
void	bt_scasti_mod_raw_t::notify_data(const datum_t &data)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// compute the offset *after* this data
	file_size_t	post_offset	= scasti_vapi()->cur_offset() + data.size();

	// put all bt_cast_spos_t contained in the stream from m_last_spos_offset to post_offset
	// - NOTE: unlikely to put several but handle it just in case
	file_size_t	next_offset	= m_last_spos_offset + m_profile.spos_maxlen();
	for(; next_offset < post_offset; next_offset += m_profile.spos_maxlen() )
		cast_spos_queue(next_offset);
}

/** \brief pop up a allowed start position - or null bt_cast_spos_t if none is available
 */
bt_cast_spos_t	bt_scasti_mod_raw_t::cast_spos_pop()		throw()
{
	// if no cast_spos_arr is queueed, return a null bt_cast_spos_t
	if( m_cast_spos_arr.empty() )	return bt_cast_spos_t();
	// unqueue the first cast_spos from the m_cast_spos_arr
	bt_cast_spos_t	cast_spos	= m_cast_spos_arr.pop_front();
	// return the result
	return cast_spos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////.

/** \brief Queue a new bt_cast_spos_t at the byte_offset
 */
void	bt_scasti_mod_raw_t::cast_spos_queue(const file_size_t &byte_offset)	throw()
{
	// append this byte_offset as bt_cast_spos_t to the m_cast_spos_arr
	bt_cast_spos_t	cast_spos(byte_offset, date_t::present());
	m_cast_spos_arr.append(cast_spos);
	// log to debug
	KLOG_DBG("appended " << cast_spos);
	// update the m_last_spos_offset
	m_last_spos_offset	= byte_offset;
	// restart the spos_timeout
	spos_timeout.start(m_profile.spos_maxdelay(), this, NULL );
	// launch the zerotimer if not already running
	if( event_zerotimer.empty() )	event_zerotimer.append(this, NULL);
}

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_scasti_mod_raw_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// notify the caller - that the bt_scasti_event_t::MOD_UPDATED
	bt_scasti_event_t scasti_event	= bt_scasti_event_t::build_mod_updated();
	bool	tokeep	= m_scasti_vapi->mod_vapi_notify_callback(scasti_event);
	if( !tokeep )	return false;

	// return a tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			spos_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	bt_scasti_mod_raw_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout) throw()
{
	// log to debug
	KLOG_ERR("enter");

	// queue the current offset of bt_scasti_vapi_t in the cast_spos_arr
	cast_spos_queue( scasti_vapi()->cur_offset() );

	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END





