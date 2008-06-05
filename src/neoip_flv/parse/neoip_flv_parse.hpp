/*! \file
    \brief Header of the flv_parse_t
    
*/


#ifndef __NEOIP_FLV_PARSE_HPP__ 
#define __NEOIP_FLV_PARSE_HPP__ 
/* system include */
/* local include */
#include "neoip_flv_parse_wikidbg.hpp"
#include "neoip_flv_parse_state.hpp"
#include "neoip_flv_parse_cb.hpp"
#include "neoip_flv_parse_profile.hpp"
#include "neoip_file_size.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	flv_err_t;

/** \brief class definition for flv_parse
 */
class flv_parse_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t
		, private wikidbg_obj_t<flv_parse_t, flv_parse_wikidbg_init>  {
private:
	flv_parse_state_t	m_state;	//!< the current flv_parse_state_t
	file_size_t		m_parsed_length;//!< the amount of data already parsed
	bytearray_t		m_buffer;	//!< the buffer bytearray_t
	flv_parse_profile_t	m_profile;	//!< the profile to use for this flv_parse_t
	
	/*************** parsing function	*******************************/
	bool			parse_buffer()		throw();
	bool			parse_tophd(bool *stop_parsing)		throw();
	bool			parse_tag(bool *stop_parsing)		throw();

	/*************** zerotimer	***************************************/
	zerotimer_t		zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer
						, void *userptr)	throw();

	/*************** callback stuff	***************************************/
	flv_parse_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const flv_parse_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	flv_parse_t() 		throw();
	~flv_parse_t()		throw();

	/*************** setup function	***************************************/
	flv_parse_t &	profile(const flv_parse_profile_t &profile)	throw();
	flv_err_t	start(flv_parse_cb_t *callback, void *userptr) 	throw();

	/*************** query function	***************************************/
	const flv_parse_state_t& state()	const throw()	{ return m_state;		} 
	const flv_parse_profile_t & profile()	const throw()	{ return m_profile;		}
	const file_size_t &	parsed_length()	const throw()	{ return m_parsed_length;	}
	const bytearray_t &	buffer()	const throw()	{ return m_buffer;		}

	/*************** action function	*******************************/
	void		notify_data(const datum_t &new_data)		throw();
	
	/*************** List of friend class	*******************************/
	friend class	flv_parse_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_PARSE_HPP__  */



