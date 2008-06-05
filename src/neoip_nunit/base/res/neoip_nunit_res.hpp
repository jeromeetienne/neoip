/*! \file
    \brief Header of the nunit_res_t object
*/


#ifndef __NEOIP_NUNIT_RES_HPP__ 
#define __NEOIP_NUNIT_RES_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_nunit_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to contain the result of a test function
 */
class nunit_res_t {
private:
	nunit_err_t	error_val;		//!< contain the error type 
	int		line_val;		//!< contain the line number of where the error has been created
	std::string	filename_val;		//!< contains the filename of where the error has been created
	std::string	fctname_val;		//!< contains the function name of where the error has been created
public:
	/*************** ctor/dtor	***************************************/
	nunit_res_t()	throw()	{}
	nunit_res_t(const nunit_err_t &error, int line = 0, const std::string &filename = std::string()
					, const std::string &fctname = std::string())	throw();


	/**************** query function	*******************************/
	bool	is_null()	const throw()	{ return error_val.is_null();	}

	/**************** get/set function for each member	***************/
	const nunit_err_t &	error()	const throw()	{ return error_val;	}
	nunit_res_t &		error(const nunit_err_t &error)	throw()
				{ error_val = error;	return *this;	}

	const int &		line()		const throw()	{ return line_val;	}
	nunit_res_t &		line(int line)	throw()		{ line_val = line; return *this;}

	const std::string &	filename()		const throw()	{ return filename_val;	}
	nunit_res_t &		filename(const std::string &filename)	throw()
				{ filename_val = filename;	return *this;	}

	const std::string &	fctname()		const throw()	{ return fctname_val;	}
	nunit_res_t &		fctname(const std::string &fctname)	throw()
				{ fctname_val = fctname;	return *this;	}

	/*************** display function	*******************************/
	std::string		to_string()		const throw();
friend	std::ostream & operator << (std::ostream & os, const nunit_res_t &nunit_res) throw()
		{ os << nunit_res.to_string(); return os;	}
};

/**************** some helper functions	***************************************/
#define NUNIT_RES_T(error)	nunit_res_t(error, __LINE__, __FILE__, std::string(__func__))
#define NUNIT_RES_OK		nunit_res_t(nunit_err_t::OK, __LINE__, __FILE__, std::string(__func__))
#define NUNIT_RES_DELAYED	nunit_res_t(nunit_err_t::DELAYED, __LINE__, __FILE__, std::string(__func__))
#define NUNIT_RES_ERROR		nunit_res_t(nunit_err_t::ERROR, __LINE__, __FILE__, std::string(__func__))

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_RES_HPP__  */



