/*! \file
    \brief Special header to include in a .hpp when using the xmlrpc serialization
    
- this include the minimum and thus reduce the dependency to decrease the compile time

*/


#ifndef __NEOIP_XMLRPC_DECL_HPP__ 
#define __NEOIP_XMLRPC_DECL_HPP__ 

#include "neoip_xml_except.hpp"

// list of forward declaration
class	xmlrpc_build_t;
class	xmlrpc_parse_t;

#include "neoip_xmlrpc.hpp"	// TODO in theory this include is explicitly made to avoid this include
				// - unfortunatly some compilation problem occurs when i dont do it
				// - i dunno why
				// - something about << >> being unable to determine the type
				// - so i keep the xmlrpc_decl stuff to build the rest of the source ok
				// - and when i have time i can sort this line out

#endif // __NEOIP_XMLRPC_DECL_HPP__ 

