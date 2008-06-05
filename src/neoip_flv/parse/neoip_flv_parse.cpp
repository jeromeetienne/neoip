/*! \file
    \brief Class to handle the flv_parse_t

\par Brief Description
flv_parse_t is done to parse FLV flash file, typically containing video or audio.
It is parsing it as a 'stream' aka it doesnt read the data by itself. The caller
has to notify the data of the FLV data. This allows to parse FLV file coming
from a network connection for example.
- it parse the input stream as a FLV file and notify flv_parse_event_t to the callback
- http://www.osflash.org/flv for a description of the FLV format

*/

/* system include */
/* local include */
#include "neoip_flv_parse.hpp"
#include "neoip_flv_parse_event.hpp"
#include "neoip_flv_err.hpp"
#include "neoip_flv_tophd.hpp"
#include "neoip_flv_taghd.hpp"
#include "neoip_flv_taghd_audio.hpp"
#include "neoip_flv_taghd_video.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
flv_parse_t::flv_parse_t()	throw()
{
	// init some variables
	m_state		= flv_parse_state_t::TOPHD;
	m_parsed_length	= 0;
}

/** \brief Destructor
 */
flv_parse_t::~flv_parse_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      Setup function  
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
flv_parse_t &	flv_parse_t::profile(const flv_parse_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= profile;
	// return the object iself
	return *this;
}
/** \brief Start the operation
 */
flv_err_t	flv_parse_t::start(flv_parse_cb_t *callback, void *userptr) 	throw()
{
	// copy the parameters
	this->callback	= callback;
	this->userptr	= userptr;
		
	// return no error
	return flv_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      action function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify new data into the flv_parse_t
 * 
 * @return a flv_err_t. if it failed, it is syntax error. aka it should not happen
 *         in 'normal' operation.
 */
void	flv_parse_t::notify_data(const datum_t &new_data) 	throw()
{
	// log to debug
	KLOG_DBG("enter new_data.size()=" << new_data.size());
	
	// queue the new_data into the buffer
	m_buffer.append(new_data);
	
	// launch the zerotimer if not already running
	if( zerotimer.empty() )	zerotimer.append(this, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////.

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	flv_parse_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// just forward to the parsing function
	return parse_buffer();
}

/** \brief parse the buffer
 * 
 * @return a tokeep for the flv_parse_t
 */
bool	flv_parse_t::parse_buffer() 	throw()
{	
	// handle the parsing differently depending on the current flv_parse_state_t
	while( true ){
		bool	stop_parsing	= false;
		// call the handler depending the current flv_parse_state_t
		switch( state().get_value() ){
		case flv_parse_state_t::TOPHD:{	bool	tokeep	= parse_tophd(&stop_parsing);
						if( !tokeep )	return false;
						break;}
		case flv_parse_state_t::TAG:{	bool	tokeep	= parse_tag(&stop_parsing);
						if( !tokeep )	return false;
						break;}
		default:	DBG_ASSERT( 0 );
		}
		// if stop_parsing has been set to true, leave the loop
		if( stop_parsing )	break;
	};

	// if the buffer is > than the max, free it and notify an error
	// - this avoid a DoS in case of bogus input 
	if( buffer().size() > profile().buffer_maxlen() ){
		m_buffer	= bytearray_t();
		flv_err_t	flv_err(flv_err_t::ERROR, "receiving buffer larger than " + OSTREAMSTR(profile().buffer_maxlen()));
		return	notify_callback( flv_parse_event_t::build_error(flv_err) );
	}

	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parser for each state
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the buffer when the state is flv_parse_state_t::TOPHD
 * 
 * - if tokeep is returned, *stop_parsing returns true if parsing must be stopped, false otherwise 
 * - if dontkeep is returned, *stop_parsing is ignored
 * 
 * @return a tokeep for flv_parse_t
 */
bool	flv_parse_t::parse_tophd(bool *stop_parsing)	throw()
{
	// sanity check - flv_parse_state_t MUST be flv_parse_state_t::TOPHD
	DBG_ASSERT( state() == flv_parse_state_t::TOPHD );

	// if buffer doesnt contains enougth data to parse, return 'stop_parsing' with noerror
	if( buffer().length() < flv_tophd_t::TOTAL_LENGTH ){
		*stop_parsing	= true;
		return true;
	}

	// try to unserialize the data with peek (not to modify it in case of faillure)	
	flv_tophd_t	flv_tophd;
	try{
		m_buffer.unserial_peek( flv_tophd );
	}catch(const serial_except_t &e){
		std::string	reason	= "failed to parse flv_tophd_t due to " + e.what();
		flv_err_t	flv_err(flv_err_t::ERROR, reason);
		return	notify_callback( flv_parse_event_t::build_error(flv_err) );
	}
	
	// notify the result to the callback
	bool	tokeep	= notify_callback( flv_parse_event_t::build_tophd(flv_tophd) );
	if( !tokeep )	return false;
	
	// consume the flv_tophd_t from m_buffer - as it has been successfully unserialized
	m_buffer.head_free( flv_tophd_t::TOTAL_LENGTH );
	// update the parsed_length
	m_parsed_length	+= flv_tophd_t::TOTAL_LENGTH;
	// switch to the next state - as flv_tophd_t is ALWAYS followed by a tag
	m_state		= flv_parse_state_t::TAG;
	
	// log for debug
	KLOG_DBG("flv_tophd="<< flv_tophd);
	// return 'tokeep' and 'continue parsing'
	*stop_parsing	= false;
	return true;
}

/** \brief Parse the buffer when the state is flv_parse_state_t::TAGHD
 * 
 * - if tokeep is returned, *stop_parsing returns true if parsing must be stopped, false otherwise 
 * - if dontkeep is returned, *stop_parsing is ignored
 * 
 * @return a tokeep for flv_parse_t
 */
bool	flv_parse_t::parse_tag(bool *stop_parsing)	throw()
{
	// sanity check - flv_parse_state_t MUST be flv_parse_state_t::TAG
	DBG_ASSERT( state() == flv_parse_state_t::TAG );
	// if buffer doesnt contains enougth data to parse, return 'stop_parsing' with noerror
	if(buffer().length() < flv_taghd_t::TOTAL_LENGTH){
		*stop_parsing	= true;
		return true;
	}

	// try to unserialize the data with peek (not to modify it in case of faillure)	
	flv_taghd_t	flv_taghd;
	try{
		m_buffer.unserial_peek( flv_taghd );
	}catch(const serial_except_t &e){
		std::string	reason	= "failed to parse flv_taghd_t due to " + e.what();
		flv_err_t	flv_err(flv_err_t::ERROR, reason);
		return	notify_callback( flv_parse_event_t::build_error(flv_err) );
	}
	
	// determine the total_len of this tag 
	size_t	total_len	= flv_taghd_t::TOTAL_LENGTH + flv_taghd.body_length();

	// if the *WHOLE* body has not yet been received, return 'stop_parsing'
	if( m_buffer.size() < total_len ){
		*stop_parsing	= true;
		return true;
	}
	
	// extract the tag_data from the 
	datum_t	tag_data = m_buffer.range(flv_taghd_t::TOTAL_LENGTH, flv_taghd.body_length());
	
	// notify the result to the callback
	bool	tokeep	= notify_callback( flv_parse_event_t::build_tag(flv_taghd, tag_data) );
	if( !tokeep )	return false;

	// NOTE: removing the tag data from the buffer is done AFTER the notification
	// - this is done on purpose, as flv_parse_helper_t::kframe_boffset_from
	//   rely on this to determine the byte position of the keyframe
	
	// consume the flv_taghd_t from m_buffer - as it has been successfully unserialized
	m_buffer.head_free( total_len );
	// update the parsed_length
	m_parsed_length	+= total_len;
	
	// return 'tokeep' and 'continue parsing'
	*stop_parsing	= false;
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool flv_parse_t::notify_callback(const flv_parse_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_flv_parse_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





