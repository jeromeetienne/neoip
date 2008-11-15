/*! \file
    \brief Header of the \ref bt_scasti_event_t
*/


#ifndef __NEOIP_BT_SCASTI_EVENT_HPP__ 
#define __NEOIP_BT_SCASTI_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

// list of include for the event parameters
#include "neoip_file_size.hpp"


NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event from bt_httpi_t
 */
class bt_scasti_event_t: NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		CHUNK_AVAIL,	//!< to notify a new data chunk from the http connection
		ERROR,		//!< when an error happened in the bt_httpi_t or bt_scasti_mod_vapi_t
		MOD_UPDATED,	//!< when the bt_scasti_mod_vapi_t data got updated
		MAX
	};
private:
	bt_scasti_event_t::type	type_val;

	// all event parameters
	file_size_t	chunk_len;
	bt_err_t	bt_err;
public:
	/*************** ctor/dtor	***************************************/
	bt_scasti_event_t()	throw();
	~bt_scasti_event_t()	throw();
	
	/*************** query function	***************************************/
	bt_scasti_event_t::type	get_value()	const throw()	{ return type_val;	}
	bool			is_fatal()	const throw()	{ return is_error();	}

	/*************** specific event ctor/dtor/query	***********************/
	bool			is_chunk_avail() 			const throw();
	static bt_scasti_event_t	build_chunk_avail(const file_size_t &chunk_len)	throw();
	const file_size_t &	get_chunk_avail()			const throw();

	bool			is_error() 				const throw();
	static bt_scasti_event_t	build_error(const bt_err_t &bt_err)	throw();
	const bt_err_t &	get_error()				const throw();

	bool			is_mod_updated() 			const throw();
	static bt_scasti_event_t	build_mod_updated()			throw();

	/*************** display function	*******************************/
	std::string		to_string()				const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_scasti_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SCASTI_EVENT_HPP__  */



