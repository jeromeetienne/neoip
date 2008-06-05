/*! \file
    \brief Declaration of the nipmem_tracker_t
*/


#ifndef __NEOIP_NIPMEM_TRACKER_HPP__ 
#define __NEOIP_NIPMEM_TRACKER_HPP__ 
/* system include */
#include <map>
#include <string>
/* local include */
#if 0
#	include "neoip_nipmem_tracker_wikidbg.hpp"
#endif
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

class nipmem_tracker_t;
nipmem_tracker_t *	get_global_nipmem_tracker()		throw();

/** \brief object to track allocated memory zone
 */
class nipmem_tracker_t : NEOIP_COPY_CTOR_ALLOW
#if 0
			, private wikidbg_obj_t<nipmem_tracker_t, nipmem_tracker_wikidbg_init>
#endif
			{
private:
	/*************** zone_db_t	***************************************/
	class				zone_t;
	std::map<void *, zone_t>	zone_db;	
public:
	/*************** ctor/dtor	***************************************/
	nipmem_tracker_t()	throw();
	~nipmem_tracker_t()	throw();
	
	/*************** zone_db management	*******************************/
	void		zone_insert(void *buffer_ptr, ssize_t buffer_len, bool secure_f
					, const char * filename, int lineno, const char * fct_name
					, const std::string &type_name)		throw();
	size_t		zone_remove(void *buffer_ptr, bool secure_f, const char *filename
					, int lineno, const char * fct_name
					, const std::string &type_name)		throw();

	/*************** display function	*******************************/
	void		display_all_zone()	const throw();
	void		display_summary()	const throw();

	/*************** list of friend class	*******************************/
#if 1
	friend class	nipmem_tracker_http_wikidbg_t;
#else
	friend class	nipmem_tracker_http_t;
#endif
};

/** \brief store one allocated zone
 */
class nipmem_tracker_t::zone_t : NEOIP_COPY_CTOR_ALLOW {
private:
	void *		buffer_ptr;	//!< pointer on the allocated zone
	size_t		buffer_len;	//!< length of the allocated zone
	bool		secure_f;	//!< true if the allocation is in secure memory, false otherwise

	const char *	filename;	//!< filename where the allocation occured
	int		lineno;		//!< line number in the filename where the allocate occured
	const char *	fct_name;	//!< function name in which the allocation occured
	std::string	type_name;	//!< the name of the type stored
public:
	/*************** ctor/dtor	***************************************/
	zone_t(void *buffer_ptr, ssize_t buffer_len, bool secure_f, const char * filename
				, int lineno, const char * fct_name, const std::string &type_name) throw()
				: buffer_ptr(buffer_ptr), buffer_len(buffer_len), secure_f(secure_f)
				, filename(filename), lineno(lineno), fct_name(fct_name)
				, type_name(type_name) {}
	/*************** display function	*******************************/						
	std::string	to_string()	const throw();

	/*************** list of friend class	*******************************/
	friend class	nipmem_tracker_t;
#if 1
	friend class	nipmem_tracker_http_wikidbg_t;
#else
	friend class	nipmem_tracker_http_t;
#endif
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NIPMEM_TRACKER_HPP__  */



