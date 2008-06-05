/*! \file
    \brief Declaration of the \ref log_layer_t

*/


#ifndef __NEOIP_LOG_LAYER_HPP__ 
#define __NEOIP_LOG_LAYER_HPP__ 
/* system include */
/* local include */
#include "neoip_log_level.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class property_t;

/** \brief layer to allocate logory in neoip
 */
class log_layer_t : NEOIP_COPY_CTOR_ALLOW {
private:
	log_level_t	default_level;
	property_t *	prop_file;
	property_t *	prop_category;

	/*************** internal function	*******************************/
	log_level_t	create_category_level(const std::string &category_name)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	log_layer_t(const log_level_t &default_level = log_level_t::INFO)	throw();
	~log_layer_t()								throw();

	/*************** query function	***************************************/
	log_level_t	find_category_level(const char *category_name)		throw();	

	/*************** action function	*******************************/
	bool		load_category_file(const std::string &filename)		throw();
	void		do_log(const char *filename, int lineno, const char *fct_name
				, const char *category_name, log_level_t log_level
				, const std::string &str )			throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LOG_LAYER_HPP__  */



