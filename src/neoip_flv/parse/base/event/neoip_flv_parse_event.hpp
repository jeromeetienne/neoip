/*! \file
    \brief Header of the \ref flv_parse_event_t
*/


#ifndef __NEOIP_FLV_PARSE_EVENT_HPP__ 
#define __NEOIP_FLV_PARSE_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_flv_err.hpp"
#include "neoip_flv_tophd.hpp"
#include "neoip_flv_taghd.hpp"
#include "neoip_datum.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event from flv_parse_t
 */
class flv_parse_event_t: NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		ERROR,
		TOPHD,
		TAG,
		MAX
	};
private:
	flv_parse_event_t::type	type_val;

	// all event parameters
	flv_err_t	flv_err;	//!< only for flv_parse_event_t::ERROR
	flv_tophd_t	tophd;		//!< only for flv_parse_event_t::TOPHD
	flv_taghd_t	taghd;		//!< only for flv_parse_event_t::TAG
	datum_t		tagdata;	//!< only for flv_parse_event_t::TAG
public:
	/*************** ctor/dtor	***************************************/
	flv_parse_event_t()	throw();
	~flv_parse_event_t()	throw();
	
	/*************** query function	***************************************/
	flv_parse_event_t::type	value()		const throw()	{ return type_val;	}
	bool			is_null()	const throw()	{ return type_val==NONE;}
	bool			is_fatal()	const throw()	{ return is_error();	}
	size_t			byte_length()	const throw();

	/*************** specific event ctor/dtor/query	***********************/
	bool			is_error() 				const throw();
	static flv_parse_event_t build_error(const flv_err_t &flv_err)	throw();
	const flv_err_t &	get_error()				const throw();

	bool			is_tophd() 				const throw();
	static flv_parse_event_t build_tophd(const flv_tophd_t &tophd)	throw();
	const flv_tophd_t &	get_tophd()				const throw();

	bool			is_tag() 				const throw();
	static flv_parse_event_t build_tag(const flv_taghd_t &taghd
					, const datum_t &tagdata)	throw();
	const flv_taghd_t &	get_tag(datum_t *tagdata_out)		const throw();
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const flv_parse_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_PARSE_EVENT_HPP__  */



