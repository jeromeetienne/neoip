/*! \file
    \brief Header of the ntudp_npos_event_t
*/


#ifndef __NEOIP_NTUDP_NPOS_EVENT_HPP__ 
#define __NEOIP_NTUDP_NPOS_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event notified by the ntudp_npos layer
 */
class ntudp_npos_event_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** list of all events	*******************************/
	enum type {
		NONE,
		COMPLETED,		//!< report a successull completion with a boolean as result
					//!< (true mean the test is positive, false means negative)
		NETWORK_ERROR,		//!< contain the reason which has been reported for the error
		TIMEDOUT,		//!< contain the reason which has been reported for the error
		MAX
	};
private:
	/*************** parameter attached to the events	***************/
	std::string	reason;			//!< parameter for NETWORK_ERROR | TIMEDOUT
	bool		completed_result;	//!< parameter for COMPLETED

	ntudp_npos_event_t::type	type_val;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_event_t()	throw();
	~ntudp_npos_event_t()	throw();

	/*************** query value	***************************************/
	ntudp_npos_event_t::type	get_value()	const throw()	{ return type_val;	}

	/*************** function for NETWORK_ERROR	***********************/
	bool			is_network_error() 					const throw();
	static ntudp_npos_event_t	build_network_error(const std::string &reason="")	throw();
	const std::string &	get_network_error_reason()			const throw();

	/*************** function for COMPLETED	***********************/
	bool			is_completed() 				const throw();
	static ntudp_npos_event_t	build_completed(bool result) 		throw();
	bool			get_completed_result()			const throw();

	/*************** function for TIMEDOUT	***********************/
	bool			is_timedout() 				const throw();
	static ntudp_npos_event_t	build_timedout(const std::string &reason="")	throw();
	const std::string &	get_timedout_reason()			const throw();


	/*************** function to sanity check	***********************/
	/** \brief return true if the event if fatal (aka if the udp object is no more usable)
	 */
	bool is_fatal()	const throw()	{ return is_network_error() || is_timedout();	}

	//! return true is the event is allowed to be returned by a ntudp_npos_saddrecho_t
	bool is_npos_saddrecho_ok() const throw() {
		// NOTE: here the is_completed() result is always true
		return is_fatal() || is_completed();
	}

	//! return true is the event is allowed to be returned by a ntudp_npos_inetreach_t
	bool is_npos_inetreach_ok() const throw() {
		return is_fatal() || is_completed();
	}

	//! return true is the event is allowed to be returned by a ntudp_npos_natlback_t
	bool is_npos_natlback_ok() const throw() {
		return is_fatal() || is_completed();
	}

	/*************** Display function	*******************************/
	std::string	to_string()						const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_npos_event_t &ntudp_npos_event)
										throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_EVENT_HPP__  */



