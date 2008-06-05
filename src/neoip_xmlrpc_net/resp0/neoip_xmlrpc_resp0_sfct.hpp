/*! \file
    \brief Header of the xmlrpc_resp0_sfct_t

- TODO to refactor the naming
  - all the naming is poorly done
    - because i coded it with no brain and dont feel like thinking about naming now
  - find better one    
  - NEOIP_XMLRPC_RESP0_SFCT_DEFINITION	=> NEOIP_RPC_SFCT_DB_DEFINITION
    - the same for DECLARATION and INSERTER 
  - NEOIP_XMLRPC_RESP0_SFCT_ITEM		=> NEOIP_RPC_SFCT_ITEM

- TODO fix the issue of the userptr and the fact that it implies to have
  a given sfct_db used only in once xmlrpc_resp0
  - the possible solution implies to copy the sfct_db in the xmlrpc_resp0_t
  - maybe to get an allocated object which will register in xmlrpc_resp0_t
    on ctor and unregister on dtor
    - to have the database in static
    - to have the userptr in the hook
    - YES would be better

*/


#ifndef __NEOIP_XMLRPC_RESP0_SFCT_HPP__ 
#define __NEOIP_XMLRPC_RESP0_SFCT_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	xmlrpc_parse_t;
class	xmlrpc_build_t;

/** \brief Type for the populate_fct_t of xmlrpc_resp0_sfct_t
 */
typedef void (*xmlrpc_resp0_sfct_addr_t)(void *userptr, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build);

/** \brief class to store and run xmlrpc_resp0_sfct_addr_t
 */
class xmlrpc_resp0_sfct_db_t {
public:
	typedef std::map<std::string, xmlrpc_resp0_sfct_addr_t>	db_t;
	db_t	db;		//!< contains all the xmlrpc_resp0_sfct_addr_t index by their name
	void *	userptr;	//!< the userptr sent to all the function of this xmlrpc_resp0_sfct_db_t
				//!< TODO this implies it is used in a single xmlrpc_rpc_t
				//!< - BAD stuff, refactor it as noted in this file header
};

/** \brief Insert a xmlrpc_resp0_sfct_addr_t in xmlrpc_resp0_sfct_t during the initialization of static
 * 
 * - The whole purpose of xmlrpc_resp0_sfct_inserter_t is to use the dummy ctor trick
 *   to 'fix' the classic ordering issue of static ctor in c++.
 * - xmlrpc_resp0_sfct_inserter_t is a phony object used to execute code during
 *   the initialization of 'static' before reaching 'main()'
 * - it uses a dummy constructor 
 * - the xmlrpc_resp0_sfct_inserter_t has no other purposes
 */
class xmlrpc_resp0_sfct_inserter_t {
public:
	/** \brief constructor
	 */
	xmlrpc_resp0_sfct_inserter_t(xmlrpc_resp0_sfct_db_t **sfct_db, const std::string &fct_name
					, xmlrpc_resp0_sfct_addr_t fct_addr)	throw()
	{
		// if the xmlrpc_resp0_sfct_db_t is not yet created, create it now
		if( *sfct_db == NULL )	*sfct_db	= new xmlrpc_resp0_sfct_db_t();
		// insert this gsuite_fct/index in the xmlrpc_resp0_sfct
		bool	succeed	= (*sfct_db)->db.insert(std::make_pair(fct_name, fct_addr)).second;
		DBG_ASSERT( succeed );
	}
};

/** \brief Define to insert a xmlrpc_resp0_sfct_addr_t into a xmlrpc_resp0_sfct_t at initialization time
 * 
 * - As the name of the variable is useless anyway - some funky stuff to create a unique name.
 *   - NOTE: i tried some anonymous namespace stuff... but i failed... not sure if it is supposed
 *     to work or if it is me doing it wrong... from the info i got, it is not supposed to 
 *     work anyway
 */
#define NEOIP_XMLRPC_RESP0_SFCT_INSERTER(SFCT_DB, FCT_NAME, FCT_ADDR)				\
	xmlrpc_resp0_sfct_inserter_t	neoip_xmlrpc_resp0_sfct_inserter_ ## SFCT_DB ## _ ## FCT_NAME(&SFCT_DB, #FCT_NAME, FCT_ADDR)

//! helper for the declaration a xmlrpc_resp0_sfct_t
#define NEOIP_XMLRPC_RESP0_SFCT_DECLARATION(SFCT_DB)		extern xmlrpc_resp0_sfct_db_t *SFCT_DB
//! helper for the definition a xmlrpc_resp0_sfct_t
#define NEOIP_XMLRPC_RESP0_SFCT_DEFINITION(SFCT_DB)		xmlrpc_resp0_sfct_db_t *SFCT_DB	= NULL

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			macro for sfct_item
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//! the #define to generate the code to parse a single parameter of a given type
#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_B(PARAM_TYPE, PARAM_NAME)		\
	PARAM_TYPE	PARAM_NAME;							\
	try {										\
		xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEG;				\
		xmlrpc_parse		>> PARAM_NAME;					\
		KLOG_DBG(#PARAM_NAME << "=" << PARAM_NAME);				\
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_END;				\
	} catch(xml_except_t &e) {							\
		xmlrpc_build	<< xmlrpc_build_t::FAULT(-1, "invalid syntax");		\
		return;									\
	}

/*
 * NOTE: the macro to generate the param parser with the handling of various number of parameter
 */
#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_0()

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_1(PARAM_TYPE1)					\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_B(PARAM_TYPE1, value1);

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_2(PARAM_TYPE1, PARAM_TYPE2)			\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_1(PARAM_TYPE1);					\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_B(PARAM_TYPE2, value2);				\

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_3(PARAM_TYPE1, PARAM_TYPE2, PARAM_TYPE3)	\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_2(PARAM_TYPE1, PARAM_TYPE2);			\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_B(PARAM_TYPE2, value3);				\

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_ALL(PARAM_NB, PARAM_LIST)			\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_##PARAM_NB PARAM_LIST


/*
 * NOTE: the macro to generate the code for a given parameter of a function call with the
 *       handling of various number of parameter
 */
#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_B(PARAM_NAME)	PARAM_NAME

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_0()

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_1()		\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_0() ,		\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_B(value1)

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_2()		\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_1() ,		\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_B(value2)

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_3()		\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_2() ,		\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_B(value3)

#define NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_ALL(PARAM_NB)	\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_##PARAM_NB()


/*
 * main macro to generate the xmlrpc-to-native convertion and to insert it in a xmlrpc_resp0_sfct_db_t
 */
#define NEOIP_XMLRPC_RESP0_SFCT_ITEM(FCT_NAME, PARAM_NB, SFCT_DB, RET_TYPE, SFCT_ADDR, PARAM_LIST)	\
namespace {												\
void	xmlrpc_resp0_fct_cvt_ ## SFCT_DB ## _ ## FCT_NAME(void *userptr, xmlrpc_parse_t &xmlrpc_parse	\
							, xmlrpc_build_t &xmlrpc_build)	throw()		\
{													\
	NEOIP_XMLRPC_RESP0_SFCT_ITEM_PARSE_PARAM_ALL(PARAM_NB, PARAM_LIST);				\
	/* create the return value with the RET_TYPE */							\
	RET_TYPE	ret_value;									\
	/* Call the sfct_addr with the converted parameters */						\
	ret_value	= (*(SFCT_ADDR))(userptr NEOIP_XMLRPC_RESP0_SFCT_ITEM_CALL_PARAM_ALL(PARAM_NB));	\
	/* build the xmlrpc reply with the ret_value */							\
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;							\
	xmlrpc_build	<< ret_value;									\
	xmlrpc_build << xmlrpc_build_t::RESP_END;							\
}												\
NEOIP_XMLRPC_RESP0_SFCT_INSERTER(SFCT_DB, FCT_NAME, xmlrpc_resp0_fct_cvt_ ## SFCT_DB ## _ ## FCT_NAME);	\
};
	
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_RESP0_SFCT_HPP__  */



