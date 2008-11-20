/*! \file
    \brief Header of the rtmp_parse_t

*/


#ifndef __NEOIP_RTMP_PARSE_HPP__
#define __NEOIP_RTMP_PARSE_HPP__
/* system include */
#include <list>
/* local include */
#include "neoip_rtmp_parse_wikidbg.hpp"
#include "neoip_rtmp_parse_cb.hpp"
#include "neoip_rtmp_parse_profile.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_parse_chanctx_t;
class	rtmp_err_t;

/** \brief class definition for rtmp_parse
 */
class rtmp_parse_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t
			, private wikidbg_obj_t<rtmp_parse_t, rtmp_parse_wikidbg_init> {
private:
	bytearray_t		m_buffer;	//!< the buffer bytearray_t
	rtmp_parse_profile_t	m_profile;	//!< the profile to use for this rtmp_parse_t

	/*************** store the rtmp_parse_chanctx_t	***********************/
	std::list<rtmp_parse_chanctx_t *>	chanctx_db;
	void chanctx_dolink(rtmp_parse_chanctx_t *chanctx)	throw()	{ chanctx_db.push_back(chanctx);}
	void chanctx_unlink(rtmp_parse_chanctx_t *chanctx)	throw()	{ chanctx_db.remove(chanctx);	}
	rtmp_parse_chanctx_t *	chanctx_by_channel_id(const uint8_t &channel_id)	throw();

	/*************** parsing function	*******************************/
	bool			parse_buffer()				throw();
	bool			parse_packet(bool *stop_parsing)	throw();

	/*************** zerotimer	***************************************/
	zerotimer_t		zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer
						, void *userptr)	throw();

	/*************** callback stuff	***************************************/
	rtmp_parse_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const rtmp_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	rtmp_parse_t() 		throw();
	~rtmp_parse_t()		throw();

	/*************** setup function	***************************************/
	rtmp_parse_t &	profile(const rtmp_parse_profile_t &profile)		throw();
	rtmp_err_t	start(rtmp_parse_cb_t *callback, void *userptr) 	throw();

	/*************** query function	***************************************/
	const rtmp_parse_profile_t & profile()	const throw()	{ return m_profile;		}
	const bytearray_t &	buffer()	const throw()	{ return m_buffer;		}

	/*************** action function	*******************************/
	void		notify_data(const datum_t &new_data)		throw();

	/*************** List of friend class	*******************************/
	friend class	rtmp_parse_wikidbg_t;
	friend class	rtmp_parse_chanctx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_PARSE_HPP__  */



