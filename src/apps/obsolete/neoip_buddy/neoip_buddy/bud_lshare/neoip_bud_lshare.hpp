/*! \file
    \brief Header of the \ref bud_lshare_t class

*/


#ifndef __NEOIP_BUD_LSHARE_HPP__ 
#define __NEOIP_BUD_LSHARE_HPP__ 
/* system include */
#include <vector>
#include <map>
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class bud_lshare_t;

typedef std::map<std::string, bud_lshare_t>	bud_lshare_db_t;


/** \brief To handle the service of buddy
 */
class bud_lshare_t : NEOIP_COPY_CTOR_ALLOW {
private:
	file_path_t				file_rootpath_val;
	std::vector<std::string>		keyword_db_val;
	std::string				local_id_val;
	std::map<std::string, std::string>	user_db_val;

	void	init_from_file(const file_path_t &file_path)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bud_lshare_t(const file_path_t &file_path)	throw();
	~bud_lshare_t()					throw();
	
	bool	is_null()	const throw() { return file_rootpath_val.is_null();	}

	/*************** query/set function	*******************************/
	file_path_t &			file_rootpath()	throw()		{ return file_rootpath_val;	}
	const file_path_t &		file_rootpath()	const throw()	{ return file_rootpath_val;	}
	std::vector<std::string> &	keyword_db()	throw()		{ return keyword_db_val;	}
	const std::vector<std::string> &keyword_db()	const throw()	{ return keyword_db_val;	}
	std::string &			local_id()	throw()		{ return local_id_val;		}
	const std::string &		local_id()	const throw()	{ return local_id_val;		}
	std::map<std::string, std::string> &		user_db()throw(){ return user_db_val;		}
	const std::map<std::string, std::string> &	user_db()const throw()	{ return user_db_val;	}

	/*************** display function	*******************************/
	std::string	to_string()		const throw();
friend	std::ostream &	operator << (std::ostream & os, const bud_lshare_t &bud_lshare)	throw()
		{ return os << bud_lshare.to_string();	}

	/*************** static functions	*******************************/
static bud_lshare_db_t	read_from_directory(const file_path_t &dir_path)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BUD_LSHARE_HPP__  */



