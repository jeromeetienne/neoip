/*! \file
    \brief Definition of the \ref xmlrpc_client_helper_t class

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_client_helper.hpp"
#include "neoip_xmlrpc_parse.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			has_succeed
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return an error if there is an error at network or xmlrpc level
 *
 * - dedicated to be used when testing for error in xmlrpc_client_cb_t
 */
xmlrpc_err_t	xmlrpc_client_helper_t::has_succeed(const xmlrpc_err_t &cb_xmlrpc_err
						, const datum_t &xmlrpc_resp)	throw()
{
	xmlrpc_err_t	xmlrpc_err;
	// log to debug
	KLOG_DBG("enter xmlrpc_err=" << cb_xmlrpc_err << " xmlrpc_resp=" << xmlrpc_resp.to_stdstring());
	// handle the error case
	if( cb_xmlrpc_err.failed() )	return cb_xmlrpc_err;

	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_resp);
	// if it is impossible to parse the xml, return an error
	if( xmlrpc_parse.is_null() )	return xmlrpc_err_t(xmlrpc_err_t::ERROR, "unable to parse the xml");

	// handle the fault case
	if( xmlrpc_parse.is_fault_resp() ){
		int32_t		fault_code;
		std::string	fault_string;
		try {
			xmlrpc_parse >> xmlrpc_parse_t::FAULT(fault_code, fault_string);
		} catch(xml_except_t &e) {
			return xmlrpc_err_t(xmlrpc_err_t::ERROR, "unable to parse the xmlrpc fault");
		}
		std::string reason	= "XMLRPC fault code="	+ OSTREAMSTR(fault_code)
								+ " reason=" + fault_string;
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, reason);
	}

	// return no error
	return xmlrpc_err_t::OK;
}


NEOIP_NAMESPACE_END


