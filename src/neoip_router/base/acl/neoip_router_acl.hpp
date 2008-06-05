

#ifndef __NEOIP_ROUTER_ACL_HPP__ 
#define __NEOIP_ROUTER_ACL_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_router_acl_wikidbg.hpp"
#include "neoip_router_acl_item.hpp"
#include "neoip_router_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	file_path_t;

/** \brief Class which implement the router_name_t ACL
 */
class router_acl_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<router_acl_t, router_acl_wikidbg_init> {
private:
	typedef std::list<router_acl_item_t>	item_db_t;
	item_db_t	item_db;
public:
	/*************** ctor/dtor	***************************************/
	router_acl_t()		throw();
	~router_acl_t()		throw();

	/*************** Query function	***************************************/
	size_t		size()				const throw()	{ return item_db.size();	}
	bool		empty()				const throw()	{ return item_db.empty();	}
	bool		is_null()			const throw()	{ return item_db.empty();	}
	bool		reject(const std::string &name)	const throw();
	const router_acl_item_t &operator[](size_t idx)	const throw();

	/*************** Update function	*******************************/
	router_acl_t &	append(const router_acl_type_t &type, const std::string &pattern)	throw();
	router_err_t	load_file(const file_path_t &file_path)					throw();

	/*************** List of friend class	*******************************/
	friend class	router_acl_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ACL_HPP__  */



