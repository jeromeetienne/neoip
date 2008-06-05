/*! \file
    \brief Definition of the \ref host2ip_agai_t class

\par Brief description
\ref host2ip_agai_t convert a hostname into a list of ip_addr_t. this particular
implementation performs it via getaddrinfo_a(). This is a rather undocumented
call specific to glibc. it uses thread.
- it uses threads and thus trigger mutex check for the WHOLE duration of the
  code once used only once.
- it is rather undocumented and seems to fails in some unknown cases
  - e.g. the code is running and suddently it is display "Killed" and die
    it seems to be a signal matter.

*/

#ifndef _WIN32

/* system include */
#include <netdb.h>
#include <sys/socket.h>
/* local include */
#include "neoip_host2ip_agai.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
host2ip_agai_t::host2ip_agai_t()	throw()
{
	// zero some field
	pending_flag	= false;
}

/** \brief Destructor
 */
host2ip_agai_t::~host2ip_agai_t()	throw()
{
	// if the operation is pending, cancel it
	if( pending_flag ){
		int	gai_err	= gai_cancel(&glibc_gai_stt);
		DBG_ASSERT( gai_err == EAI_CANCELED );
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	host2ip_agai_t::start(const std::string &m_hostname, host2ip_cb_t *callback
							, void *userptr)	throw()
{
	// copy the parameter
	this->m_hostname	= m_hostname;
	this->callback		= callback;
	this->userptr		= userptr;

	// fill the gaicb_stt
	memset(&glibc_gai_stt, 0, sizeof(glibc_gai_stt));
	glibc_gai_stt.ar_name	= glibc_gai_name;
	// copy the name parameter into the getaddrinfo
	DBG_ASSERT(hostname().size() < sizeof(glibc_gai_name));
	strncpy(glibc_gai_name, hostname().c_str(), sizeof(glibc_gai_name)-1);

	// actually launch the getaddrinfo_a
	struct gaicb *	glibc_gai_tmp	= &glibc_gai_stt;
	int	gai_err = getaddrinfo_a(GAI_NOWAIT, &glibc_gai_tmp, 1, NULL);
	if( gai_err )	return inet_err_t(inet_err_t::ERROR, gai_strerror(gai_err));

	// set the pending_flag
	pending_flag	= true;

	// configure the probe_delaygen
	probe_delaygen	= delaygen_t(delaygen_expboff_arg_t().min_delay(delay_t::from_msec(100))
							.max_delay(delay_t::from_msec(500)));
	// start the probe_timeout
	probe_timeout.start(probe_delaygen.current(), this, NULL);
	// return no error
	return inet_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool host2ip_agai_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug 
	KLOG_DBG("enter");
	
	// test if the glibc_gai_stt
	int	gai_err	= gai_error(&glibc_gai_stt);
	// if the result is now available
	if( gai_err == 0 ){
		struct addrinfo *	ar_result	= glibc_gai_stt.ar_result;
		std::vector<ip_addr_t>	result;
		// build the result from the returned glibc_gai_stt.ar_result
		for( ; ar_result ; ar_result = ar_result->ai_next){
			ip_addr_t	ip_addr;
			// convert the result as an ip_addr
			ip_addr		= ip_addr_t(*(struct sockaddr_in *)ar_result->ai_addr);
			// check if the ip_addr_t is already in the result
			size_t	i;
			for(i = 0; i < result.size() && result[i] != ip_addr; i++);
			if( i != result.size() )	continue;
			// else include it in the result
			result.push_back(ip_addr);			
		}
		// clear the pending_flag
		pending_flag	= false;
		// notify the successfull result to the caller
		return notify_callback(inet_err_t::OK, result);
	}

	// if the getaddrinfo_a failed
	if( gai_err != EAI_INPROGRESS ){
		// clear the pending_flag
		pending_flag	= false;
		// notify an error to the caller
		inet_err_t inet_err(inet_err_t::ERROR, gai_strerror(gai_err));
		return notify_callback(inet_err, std::vector<ip_addr_t>());		
	}
	
	// update the probe_timeout
	probe_timeout.change_period( probe_delaygen.pre_inc() );
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool host2ip_agai_t::notify_callback(const inet_err_t &inet_err, const std::vector<ip_addr_t> &result)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_host2ip_cb(userptr, *this, inet_err, result);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

#endif // _WIN32
