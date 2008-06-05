/*! \file
    \brief Header of the cpp_backtrace_t
*/


#ifndef __NEOIP_CPP_BACKTRACE_HPP__ 
#define __NEOIP_CPP_BACKTRACE_HPP__ 
/* system include */
#include <iostream>
#include <vector>
/* local include */
#include "neoip_cpp_location.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN


/** \ref class to store the socket itor
 */
class cpp_backtrace_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::vector<void *>	code_ptr_array;	//!< contain the pointers in the stack frame
public:
	/*************** Setup function	***************************************/
	cpp_backtrace_t&initialize()		throw();

	/*************** Query function	***************************************/
	size_t		size()			const throw()	{ return code_ptr_array.size();	}
	bool		empty()			const throw()	{ return code_ptr_array.empty();}
	cpp_location_t	operator[](int idx)	const;
	
	/*************** Display function	*******************************/
	friend	std::ostream & operator << (std::ostream & os, const cpp_backtrace_t & cpp_backtrace)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CPP_BACKTRACE_HPP__  */



