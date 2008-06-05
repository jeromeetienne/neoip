/*! \file
    \brief Header of the xmlrpc_resp_t

- see \ref neoip_xmlrpc_resp.cpp

*/


#ifndef __NEOIP_XMLRPC_EXCEPT_HPP__ 
#define __NEOIP_XMLRPC_EXCEPT_HPP__ 
/* system include */
/* local include */
#include "neoip_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

class xml_except_t : public neoip_except {
public:	
	xml_except_t(std::string reason)
			: neoip_except(reason ){}
	xml_except_t(std::string reason, std::string filename, int line, std::string fctname )
			: neoip_except(reason, filename, line, fctname ){}
};
#define xmlrpc_except_l(reason)		xml_except_t(reason, __FILE__, __LINE__, __func__)
#define nthrow_xmlrpc_errno(str)	NTHROW xmlrpc_except_l(std::string(str) \
							+ ". errno=" + neoip_strerror(errno) )
#define nthrow_xmlrpc_plain(str)	NTHROW xmlrpc_except_l(std::string(str))

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_EXCEPT_HPP__  */



