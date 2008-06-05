/*! \file
    \brief Header of the \ref ndiag_cacheport_t

*/


#ifndef __NEOIP_NDIAG_CACHEPORT_HPP__ 
#define __NEOIP_NDIAG_CACHEPORT_HPP__ 
/* system include */
/* local include */
#include "neoip_ndiag_cacheport_wikidbg.hpp"
#include "neoip_ndiag_cacheport_item.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	router_peer_t;

/** \brief An item of the ndiag_cacheport_t
 */
class ndiag_cacheport_t : NEOIP_COPY_CTOR_ALLOW
			, private wikidbg_obj_t<ndiag_cacheport_t, ndiag_cacheport_wikidbg_init> {
private:
	/*************** item database	***************************************/
	typedef	ndiag_cacheport_item_t	item_t;
	typedef	item_arr_t<item_t>	item_db_t;
	item_db_t			item_db;
public:
	/*************** ctor/dtor	***************************************/
	ndiag_cacheport_t()			throw();
	~ndiag_cacheport_t()			throw();

	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return item_db.empty();	}
	bool			empty()		const throw()	{ return item_db.empty();	}
	size_t			size()		const throw()	{ return item_db.size();	}
	const ndiag_cacheport_item_t *item_by_key(const std::string &item_key
						, const upnp_sockfam_t &item_sockfam)	const throw();
	bool			contain(const std::string &item_key
						, const upnp_sockfam_t &item_sockfam) 	const throw()
						{ return item_by_key(item_key, item_sockfam) != NULL;	}
	const ndiag_cacheport_item_t &item(const std::string &item_key
						, const upnp_sockfam_t &item_sockfam)	const throw();

	/*************** update function	*******************************/
	ndiag_cacheport_t &	update(const ndiag_cacheport_item_t &cacheport_item)	throw();
	ndiag_cacheport_t &	remove(const std::string &item_key
						, const upnp_sockfam_t &item_sockfam)	throw();

	/*************** Helper function	*******************************/
	ndiag_cacheport_t &	update(const std::string &item_key, const upnp_sockfam_t &item_sockfam
								, uint16_t item_port)	throw()
			{ return update(ndiag_cacheport_item_t(item_key, item_sockfam, item_port));	}

	/*************** display function	*******************************/
	std::string	to_string()		const throw();
	friend	std::ostream & operator << (std::ostream & os, const ndiag_cacheport_t &ndiag_cacheport)throw()
						{ return os << ndiag_cacheport.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const ndiag_cacheport_t &ndiag_cacheport)throw();
	friend	serial_t& operator >> (serial_t& serial, ndiag_cacheport_t &ndiag_cacheport)	throw(serial_except_t);	
	
	/*************** Friend class	***************************************/
	friend class	ndiag_cacheport_wikidbg_t;
	friend class	ndiag_cacheport_item_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NDIAG_CACHEPORT_HPP__  */



