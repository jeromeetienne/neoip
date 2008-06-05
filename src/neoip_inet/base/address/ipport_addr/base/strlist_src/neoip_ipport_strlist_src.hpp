/*! \file
    \brief Declaration of the ipport_strlist_src_t
    
*/


#ifndef __NEOIP_IPPORT_STRLIST_SRC_HPP__ 
#define __NEOIP_IPPORT_STRLIST_SRC_HPP__ 
/* system include */
#include <vector>
#include <list>
/* local include */
#include "neoip_ipport_strlist_src_cb.hpp"
#include "neoip_host2ip_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Act as a source of ipport_addr_t taken from a list of string
 * 
 * - NOTE: those string represents ipport_addr_t AND may contains hostname too
 */
class ipport_strlist_src_t : private zerotimer_cb_t, NEOIP_COPY_CTOR_DENY, private host2ip_cb_t {
private:
	std::vector<std::string>	strlist_db;		//!< all the strings containing the address
	std::list<ipport_addr_t>	tobedelivered_db;	//!< all the ipport_addr_t to be delivered
								//!< BEFORE using strlist_db

	size_t		cur_idx;		//!< the current index in the database
	bool		want_more;		//!< true if the caller want more
						//!< addresses. false otherwise.

	/*************** host2ip_t to resolve hostname	***********************/
	host2ip_t *	host2ip;		//!< host2ip to resolve hostname
	bool		neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &ipaddr_arr)	throw();

	/*************** zerotimer to avoid notification during user call *****/
	zerotimer_t	zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();

	/*************** data for result notification	***********************/
	ipport_strlist_src_cb_t *	callback;	//!< the callback to notify ipport_addr_t
	void *				userptr;	//!< the userptr associated with the callback
	bool		notify_callback(const ipport_addr_t &ipport_addr)	throw();	
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

	/*************** internal function	*******************************/
	bool		is_raw_ipaddr_str(const std::string &addr_str)			const throw();
	std::pair<std::string, size_t>	parse_ipport_str(const std::string &ipport_str)	const throw();
	bool		do_delivery()							throw();
public:
	/*************** ctor/dtor	***************************************/
	ipport_strlist_src_t()		throw();
	~ipport_strlist_src_t()		throw();
	
	
	/*************** Setup Function	***************************************/
	ipport_strlist_src_t &	append(const std::string &ipport_str)		throw();
	inet_err_t	append_from_file(const std::string &filename)		throw();
	inet_err_t	start(ipport_strlist_src_cb_t *callback, void *userptr)	throw();
	
	/*************** Query Function	***************************************/
	void		get_more()					throw();
	void		push_back(const ipport_addr_t &ipport_addr)	throw();

	/*************** display function	*******************************/
	std::string	to_string()					const throw();
	friend	std::ostream & operator << (std::ostream & os, const ipport_strlist_src_t &strlist_src ) throw()
							{ return os << strlist_src.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPORT_STRLIST_SRC_HPP__  */



