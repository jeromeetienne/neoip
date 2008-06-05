/*! \file
    \brief Header of the bencode_t class
    
*/


#ifndef __NEOIP_BENCODE_HPP__ 
#define __NEOIP_BENCODE_HPP__ 
/* system include */
#include <string>
#include <sstream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	dvar_t;
class	datum_t;

/** \brief helper to manipulate bencode_t format
 * 
 * - the bencode format specify that integer are signed, but not their maximum
 *   size in bit, so i choosed to put 64bit and thus not restrict neoip_bt to 
 *   2Gbyte files
 */
class bencode_t {
private:
	static dvar_t		to_dvar_iss(std::istringstream &iss)	throw();
public:
	static std::string	from_dvar(const dvar_t &dvar)		throw();
	static dvar_t		to_dvar(const std::string &str)		throw();
	static dvar_t		to_dvar(const datum_t &datum)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BENCODE_HPP__  */



