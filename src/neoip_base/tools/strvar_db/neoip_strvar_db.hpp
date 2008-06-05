/*! \file
    \brief Declaration of the strvar_db_t

*/


#ifndef __NEOIP_STRVAR_DB_HPP__ 
#define __NEOIP_STRVAR_DB_HPP__ 
/* system include */
/* local include */
#include "neoip_strvar_item.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a pool of variable key=value (both in std::string, duplicate key allowed)
 */
class strvar_db_t : public item_arr_t<strvar_item_t> {
public:
	static const size_t	INDEX_NONE;
private:
	bool		m_key_ignorecase;	//!< true if the key comparison MUST ignore case

	/*************** Internal function	*******************************/
	int		keycmp(const std::string &key1, const std::string &key2)	const throw();
public:
	/*************** ctor/dtor	***************************************/
	strvar_db_t()	throw();
	
	/*************** Query function	***************************************/
	size_t		next_key_idx(const std::string &key, size_t prev_idx)	const throw();
	size_t		first_key_idx(const std::string &key)	const throw()
							{ return next_key_idx(key, INDEX_NONE);		}
	bool		contain_key(const std::string &key)	const throw()
							{ return first_key_idx(key) != INDEX_NONE;	}
	const std::string &get_first_value(const std::string &key
				, const std::string &default_value = std::string())const throw();
	bool		key_ignorecase()	const throw()	{ return m_key_ignorecase;		}

	/*************** Action function	*******************************/
	strvar_db_t	key_ignorecase(bool new_val)	throw()	{ m_key_ignorecase = new_val; return *this;	}

	/*************** update function	*******************************/
	strvar_db_t &	append(const std::string &key, const std::string &val)	throw();
	strvar_db_t &	update(const std::string &key, const std::string &val)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_STRVAR_DB_HPP__  */



