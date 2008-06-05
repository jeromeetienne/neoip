/*! \file
    \brief Header of the cpp_location_t
*/


#ifndef __NEOIP_CPP_LOCATION_HPP__ 
#define __NEOIP_CPP_LOCATION_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN


/** \ref class to store the socket itor
 */
class cpp_location_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string	exec_fname;
	std::string	function;
public:
	/*************** ctor/dtor	***************************************/
	explicit cpp_location_t(const void *ptr) 	throw();
	
	/*************** Query function	***************************************/
	std::string	get_exec_fname()	const throw();
	std::string	get_function_name()	const throw();

	/*************** Display function	*******************************/
	friend	std::ostream & operator << (std::ostream & os, const cpp_location_t & cpp_location)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CPP_LOCATION_HPP__  */



