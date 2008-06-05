/*! \file
    \brief Definition of the \ref bt_http_ecnx_cnx_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_http_ecnx_cnx_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_http_ecnx_cnx_profile_t constant
#if 0
	const delay_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_DELAY	= delay_t::from_msec(400);
	const size_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_MINLEN	= 1*(16*1024);
	const size_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_MAXLEN	= 64*(16*1024);
	const size_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_MINLEN_CNXCLOSE_MULT
										= 8;
	const file_size_t	bt_http_ecnx_cnx_profile_t::OUTTER_REQ_MAXLEN	= 512*1024;
	const rate_estim_arg_t	bt_http_ecnx_cnx_profile_t::RATE_ESTIM_ARG	= rate_estim_arg_t()
								.total_delay(delay_t::from_sec(20))
								.slice_delay(delay_t::from_sec(2));
#else
	const delay_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_DELAY	= delay_t::from_sec(30);
	const size_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_MINLEN	= 4*(16*1024);
	const size_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_MAXLEN	= 128*(16*1024);		
	const size_t		bt_http_ecnx_cnx_profile_t::REQ_QUEUE_MINLEN_CNXCLOSE_MULT
										= 8;
	const file_size_t	bt_http_ecnx_cnx_profile_t::OUTTER_REQ_MAXLEN	= 3*1024*1024;
	const rate_estim_arg_t	bt_http_ecnx_cnx_profile_t::RATE_ESTIM_ARG	= rate_estim_arg_t()
								.total_delay(delay_t::from_sec(20))
								.slice_delay(delay_t::from_sec(2));
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_http_ecnx_cnx_profile_t::bt_http_ecnx_cnx_profile_t()	throw()
{
	// init each plain field with its default value
	req_queue_delay			(REQ_QUEUE_DELAY);
	req_queue_minlen		(REQ_QUEUE_MINLEN);
	req_queue_maxlen		(REQ_QUEUE_MAXLEN);
	req_queue_minlen_cnxclose_mult	(REQ_QUEUE_MINLEN_CNXCLOSE_MULT);
	outter_req_maxlen		(OUTTER_REQ_MAXLEN);
	rate_estim_arg			(RATE_ESTIM_ARG);
}

/** \brief Destructor
 */
bt_http_ecnx_cnx_profile_t::~bt_http_ecnx_cnx_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_http_ecnx_cnx_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

