/*! \file
    \brief Definition of the \ref bt_http_ecnx_cnx_stats_t

- TODO do a ratio ecnx_iov length vs req_queue_maxlen
  - average req_queue_fill_ratio

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_http_ecnx_cnx_stats.hpp"
#include "neoip_bt_http_ecnx_iov.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_http_ecnx_cnx_stats_t::bt_http_ecnx_cnx_stats_t()	throw()
{
	// set the initial value
	uloaded_ovhdlen	( 0 );
	dloaded_datalen	( 0 );
	dloaded_ovhdlen	( 0 );
	iov_nb		( 0 );
	iov_total_len	( 0 );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Update the bt_http_ecnx_cnx_stats_t with bt_http_ecnx_iov_t result
 * 
 * - a lot of the statistic are made from this result, so their computation are
 *   localized here
 */
void	bt_http_ecnx_cnx_stats_t::update_from_iov_res(bt_http_ecnx_iov_t *ecnx_iov
					, const http_sclient_res_t &sclient_res)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// update the iov_nb
	iov_nb		( iov_nb() + 1 );
	// update the iov_total_len
	iov_total_len	( iov_total_len() + ecnx_iov->bt_iov().length() );
	// update the uloaded_ovhdlen
	uloaded_ovhdlen	( uloaded_ovhdlen() + sclient_res.http_reqhd().to_http().size() );
	// if there is a http_rephd, update the dloaded_ovhdlen with it
	if( !sclient_res.http_rephd().is_null() )
		dloaded_ovhdlen	( dloaded_ovhdlen() + sclient_res.http_rephd().to_http().size() );
	// if the sclient_res succeed, update the dloaded_datalen with the reply_body size
	if( sclient_res.part_get_ok() )
		dloaded_datalen	( dloaded_datalen() + sclient_res.reply_body().size() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       arithmetic operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a bt_http_ecnx_cnx_stats_t to this one 
 */
bt_http_ecnx_cnx_stats_t & bt_http_ecnx_cnx_stats_t::operator +=(const bt_http_ecnx_cnx_stats_t &other)	throw()
{
	uloaded_ovhdlen	( uloaded_ovhdlen() + other.uloaded_ovhdlen() );
	dloaded_datalen	( dloaded_datalen() + other.dloaded_datalen() );
	dloaded_ovhdlen	( uloaded_ovhdlen() + other.dloaded_ovhdlen() );
	iov_nb		( iov_nb() + other.iov_nb() );
	iov_total_len	( iov_total_len() + other.iov_total_len() );
	// return the object itself
	return *this;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_http_ecnx_cnx_stats_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string to return
	oss        << "uloaded_ovhdlen="	<< uloaded_ovhdlen();
	oss << " " << "dloaded_datalen="	<< dloaded_datalen();
	oss << " " << "dloaded_ovhdlen="	<< dloaded_ovhdlen();
	oss << " " << "iov_nb="			<< iov_nb();
	oss << " " << "iov_total_len="		<< iov_total_len();
	
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END

