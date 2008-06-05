/*! \file
    \brief Header of the xmlrpc_resp_t
    
*/


#ifndef __NEOIP_XMLRPC_RESP_HPP__ 
#define __NEOIP_XMLRPC_RESP_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_xmlrpc_resp_wikidbg.hpp"
#include "neoip_xmlrpc_resp_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	xmlrpc_listener_t;
class	xmlrpc_err_t;

/** \brief class definition for xmlrpc_resp_t
 * 
 */
class xmlrpc_resp_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<xmlrpc_resp_t, xmlrpc_resp_wikidbg_init> {				
private:
	xmlrpc_listener_t *	m_xmlrpc_listener;
	std::list<std::string>	m_method_db;

	/*************** callback stuff	***************************************/
	xmlrpc_resp_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const std::string &method_name, xmlrpc_err_t &err_out
						, xmlrpc_parse_t &xmlrpc_parse
						, xmlrpc_build_t &xmlrpc_build)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_resp_t() 	throw();
	~xmlrpc_resp_t()	throw();
	
	/*************** setup function	***************************************/
	xmlrpc_err_t	start(xmlrpc_listener_t *m_xmlrpc_listener
				, xmlrpc_resp_cb_t *callback, void *userptr)	throw();

	
	/*************** Query function	***************************************/
	bool		contain_method(const std::string &method_name)	const throw();
	
	/*************** Action function	*******************************/
	xmlrpc_resp_t &	insert_method(const std::string &method_name)	throw();
	xmlrpc_resp_t &	delete_method(const std::string &method_name)	throw();

	/*************** List of friend class	*******************************/
	friend class	xmlrpc_resp_wikidbg_t;
	friend class	xmlrpc_listener_t;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			macro to ease the xmlrpc_resp_t parse/build
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define NEOIP_XMLRPC_RESP_PARSE_BEGIN(parser)			\
	try {
#define NEOIP_XMLRPC_RESP_PARSE_ARG(parser, variable)		\
		parser	>> xmlrpc_parse_t::PARAM_BEG;		\
		parser		>> variable;			\
		parser	>> xmlrpc_parse_t::PARAM_END;
#define	NEOIP_XMLRPC_RESP_PARSE_END(parser, err_out)						\
	} catch(xml_except_t &e){							\
		KLOG_ERR("xml_except_t=" << e.what());					\
		std::string	reason	= "unable to parse the xmlrpc parameter";	\
		err_out	= xmlrpc_err_t(xmlrpc_err_t::ERROR, reason );			\
	}

#define NEOIP_XMLRPC_RESP_BUILD(builder, variable)		\
		builder << xmlrpc_build_t::RESP_BEG;		\
		builder		<< variable;			\
		builder	<< xmlrpc_build_t::RESP_END;

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_RESP_HPP__  */



