/*! \file
    \brief Header of the xml_except_t

*/


#ifndef __NEOIP_XML_EXCEPT_HPP__ 
#define __NEOIP_XML_EXCEPT_HPP__ 
/* system include */
/* local include */
#include "neoip_except.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

class xml_except_t : public neoip_except {
public:	
	xml_except_t(const std::string &reason) : neoip_except(reason)	{}
	xml_except_t(const std::string &reason, const std::string &filename, int line
			, const std::string &fctname) : neoip_except(reason, filename, line, fctname){}
};

#define xml_except_l(reason)	xml_except_t(reason, __FILE__, __LINE__, __func__)
#define nthrow_xml_errno(str)	NTHROW xml_except_l(std::string(str) \
							+ ". errno=" + neoip_strerror(errno) )
#define nthrow_xml_plain(str)	NTHROW xml_except_l(str)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XML_EXCEPT_HPP__  */


