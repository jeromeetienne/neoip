/*! \file
    \brief Header of the kad_event_t

*/


#ifndef __NEOIP_KAD_EVENT_HPP__ 
#define __NEOIP_KAD_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_caddr_arr.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class pkt_t;

/** \ref class to store the event for the kademlia layer
 */
class kad_event_t {
public:
	enum type {
		NONE,
		CNX_CLOSED,	//!< when the kad_clicnx_t is closed
		CNX_REFUSED,	//!< when the kad_clicnx_t is refused
		TIMEDOUT,	//!< when the kad_clicnx_t timed out
		RECVED_DATA,	//!< when the kad_clicnx_t receives data
		COMPLETED,	//!< when the rpc is completed (used for ping and store)
		RECDUPS,	//!< when the rpc answer is a kad_recdups_t + had_more_record
		ADDR_ARR,	//!< when the rpc answers is a kad_addr_arr_t + responder cookie
		CADDR_ARR,	//!< when the rpc answers is a kad_caddr_arr_t
		MAX
	};
private:
	kad_event_t::type	type_val;

	std::string	reason_str;
	pkt_t *		pkt_ptr;
	kad_caddr_arr_t kad_caddr_arr;
	
	kad_recdups_t 	kad_recdups;
	bool		has_more_record_val;

	kad_addr_arr_t	kad_addr_arr;
	cookie_id_t	resp_cookie;	//!< valid iif it is notified from rpccli 
					//!< and attached to a event containing addr_arr
public:
	// ctor/dtor
	kad_event_t()	throw();
	~kad_event_t()	throw();

	bool			is_null()	const throw()	{ return type_val == NONE;	}
	//! to return a value from this kad_event_t
	kad_event_t::type	get_value()	const throw()	{ return type_val;		}

	bool			is_cnx_closed() 				const throw();
	static kad_event_t	build_cnx_closed(const std::string &reason)	throw();
	const std::string &	get_cnx_closed_reason()				const throw();

	bool			is_cnx_refused() 				const throw();
	static kad_event_t	build_cnx_refused(const std::string &reason) throw();
	const std::string &	get_cnx_refused_reason()			const throw();

	bool			is_timedout() 					const throw();
	static kad_event_t	build_timedout(const std::string &reason)	throw();
	const std::string &	get_timedout_reason()				const throw();
	
	bool			is_recved_data() 				const throw();
	static kad_event_t	build_recved_data(pkt_t *pkt)			throw();
	pkt_t *			get_recved_data()				const throw();
	
	bool			is_completed()	 				const throw();
	static kad_event_t	build_completed()				throw();

	bool			is_recdups() 					const throw();
	static kad_event_t	build_recdups(const kad_recdups_t &recdups, bool has_more_record)throw();
	const kad_recdups_t &	get_recdups(bool *has_more_record_out)		const throw();

	bool			is_addr_arr() 					const throw();
	static kad_event_t	build_addr_arr(const kad_addr_arr_t &addr_arr
					, const cookie_id_t &resp_cookie)	throw();
	const kad_addr_arr_t &	get_addr_arr(cookie_id_t *resp_cookie_out)	const throw();

	bool			is_caddr_arr() 					const throw();
	static kad_event_t	build_caddr_arr(const kad_caddr_arr_t &kad_caddr_arr)	throw();
	const kad_caddr_arr_t &	get_caddr_arr()					const throw();
	

//////////////////////////////////////////////////////////////////////////////
// CNX
//////////////////////////////////////////////////////////////////////////////	
	//! return true if the object is no more usable after that
	bool is_fatal()	const throw()
		{ return is_cnx_closed() || is_cnx_refused() || is_timedout(); }

	//! return true if the event is from \ref kad_clicnx_t
	bool is_cnx_ok() const throw()
		{ return is_cnx_closed() || is_cnx_refused() || is_timedout() || is_recved_data(); }

	//! return true if the event is from \ref kad_clicnx_t and report and error
	bool is_cnx_err_ok() const throw()
		{ return is_cnx_closed() || is_cnx_refused() || is_timedout(); }

//////////////////////////////////////////////////////////////////////////////
// PLAIN RPC
//////////////////////////////////////////////////////////////////////////////
	//!< return true if the event MAY be returned by a PING rpc
	bool is_ping_rpc_ok() const throw()
		{ return is_cnx_err_ok() || is_completed();			}

	//!< return true if the event MAY be returned by a STORE rpc
	bool is_store_rpc_ok() const throw()
		{ return is_cnx_err_ok() || is_completed();			}

	//!< return true if the event MAY be returned by a FINDNODE rpc
	bool is_findnode_rpc_ok() const throw()
		{ return is_cnx_err_ok() || is_addr_arr();			}

	//!< return true if the event MAY be returned by a FINDSOMEVAL rpc
	bool is_findsomeval_rpc_ok() const throw()
		{ return is_cnx_err_ok() || is_addr_arr() || is_recdups();	}

	//!< return true if the event MAY be returned by a FINDALLVAL rpc
	bool is_findallval_rpc_ok() const throw()
		{ return is_cnx_err_ok() || is_recdups();			}
		
	//!< return true if the event MAY be returned by a DELETE rpc
	bool is_delete_rpc_ok() const throw()
		{ return is_cnx_err_ok() || is_completed();			}		

//////////////////////////////////////////////////////////////////////////////
// closest node RPC
//////////////////////////////////////////////////////////////////////////////

	//!< return true if the event MAY be returned by a kad_closestnode_t
	bool is_nsearch_findnode_ok()	const throw()	{ return is_timedout() || is_caddr_arr();	}
	bool is_closestnode_ok()const throw()	{ return is_nsearch_findnode_ok();	}

	//!< return true if the event MAY be returned by a kad_query_some_t
	bool is_nsearch_findsomeval_ok()const throw()	{ return is_timedout() || is_recdups();		}
	bool is_query_some_ok()	const throw()	{ return is_nsearch_findsomeval_ok();	}

	//!< return true if the event MAY be returned by a kad_publish_t
	bool is_store_ok()	const throw()	{ return is_timedout() || is_completed();	}

	//!< return true if the event MAY be returned by a kad_delete_t
	bool is_delete_ok()	const throw()	{ return is_timedout() || is_completed();	}

	//!< return true if the event MAY be returned by a kad_query_t
	bool is_query_ok()	const throw()	{ return is_timedout() || is_recdups();		}

	//!< return true if the event MAY be returned by a kad_publish_t
	bool is_publish_ok()	const throw()	{ return is_store_ok();				}

	/*************** Display Function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const kad_event_t &kad_event) throw()
					{ return oss << kad_event.to_string();	}

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_event_t &kad_event)throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_event_t &kad_event)	throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_EVENT_HPP__  */



