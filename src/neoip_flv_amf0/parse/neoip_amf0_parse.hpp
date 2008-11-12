/*! \file
    \brief Header of the amf0_parse_helper_t class

*/


#ifndef __NEOIP_AMF0_PARSE_HPP__
#define __NEOIP_AMF0_PARSE_HPP__
/* system include */
#include <stdio.h>
/* local include */
#include "neoip_dvar.hpp"
#include "neoip_serial_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	flv_err_t;
class	dvar_t;
class	bytearray_t;
class	datum_t;

/** \brief static function to parse Actionscript data types in AMF
 */
class amf0_parse_t {
public:
	static dvar_t	parser(bytearray_t &bytearray)				throw(serial_except_t);
	static flv_err_t amf_to_dvar(bytearray_t &amf0_barray, dvar_t &dvar_out)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_AMF0_PARSE_HPP__  */



