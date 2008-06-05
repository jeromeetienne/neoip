/*! \file
    \brief Definition of the \ref host2ip_fork_t class

\par Brief description
\ref host2ip_fork_t convert a hostname into a list of ip_addr_t. this particular
implementation performs it via a fork() and pipe() and then call an external
executable to perform the gethostbyname.

\par Relation with thread
- the main advantate of this implementation is to avoid using threads
- i have great fear of thread because they are hard to debug
- thread when started cause the libc to perform thread mutex for the rest
  of the program execution, EVEN if there are no more thread running.
  - this slows down the execution
  - glib has the same issue and i measured its impact to 70% of my whole cpu 
    usage !!!!!
  - the cpu impact for glibc is unclear, but for example each malloc/free 
    requires mutex check. i did some basic measurement at 7-8% of my whole cpu.
- using fork is likely to use more rescources than thread DURING the host2ip_fork_t
  - but wont use any rescource AFTER host2ip_fork_t
- as using host2ip is rather rare in practice, the overall cost of the fork 
  implementation is likely much lower, than the thread one.

\par BUG for host with ipv6 address
- neoip-dnsresolv.sh has a bug if the queried host has a ipv6 addess.
- it relies on "getent hosts myhost.tld" to get the ipv4 address. and this
  tool is bugged because it will return only the ipv6 address is there is one
  and there is no way to request the ipv4 one.
- POSSIBLE SOLUTION: just write a small C apps/script this does an actual
  gethostbyname()

*/

/* system include */
/* local include */
#include "neoip_host2ip_fork.hpp"
#include "neoip_asyncexe.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_file_path.hpp"
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
host2ip_fork_t::host2ip_fork_t()	throw()
{
	// zero some fields
	asyncexe	= NULL;
}

/** \brief Destructor
 */
host2ip_fork_t::~host2ip_fork_t()	throw()
{
	// delete asyncexe_t if needed
	nipmem_zdelete	asyncexe;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	host2ip_fork_t::start(const std::string &m_hostname, host2ip_cb_t *callback
							, void *userptr)	throw()
{
	lib_session_t *	lib_session	= lib_session_get();
	file_path_t	exe_path	= lib_session->conf_rootdir() / "neoip-dnsresolve.sh";
	// log to debug
	KLOG_ERR("enter hostname=" << m_hostname);
	// copy the parameter
	this->m_hostname	= m_hostname;
	this->callback		= callback;
	this->userptr		= userptr;

	// build the command line to execute
	item_arr_t<std::string>	cmdline_arr;
	cmdline_arr	= item_arr_t<std::string>().append(exe_path.to_string()).append(hostname());
	// launch the asyncexe_t	
	libsess_err_t	libsess_err;
	asyncexe	= nipmem_new asyncexe_t();
	libsess_err	= asyncexe->start(cmdline_arr, this, NULL);
	if( libsess_err.failed() )	return inet_err_t(inet_err_t::ERROR, libsess_err.to_string());
	
	// return no error
	return inet_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    asyncexe_query_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when asyncexe_t has an event to report
 */
bool	host2ip_fork_t::neoip_asyncexe_cb(void *cb_userptr, asyncexe_t &cb_asyncexe
					, const libsess_err_t &libsess_err
					, const bytearray_t &stdout_barray
					, const int &exit_status)	throw()
{
	// log to debug
	KLOG_ERR("enter libsess_err="<< libsess_err << " exit_status=" << exit_status
						<< " stdout_barray=" << stdout_barray);

	// if exit_status is non null, notify the called
	if( exit_status != 0 ){
		std::string reason	= "neoip-dnsresolve.sh returned error " + OSTREAMSTR(exit_status);
		return notify_callback_err(inet_err_t(inet_err_t::ERROR, reason));
	}

	/*************** parse the output_barray	***********************/
	std::string			recved_str	= stdout_barray.to_stdstring();
	std::vector<std::string> 	arr_str		= string_t::split(recved_str, "/");
	std::vector<ip_addr_t>		arr_ipaddr;
	// parse the arr_str into arr_ipaddr
	for(size_t i = 0; i < arr_str.size(); i++){
		// if this string is empty, leave the loop
		if( arr_str[i].empty() )	break;
		// sanity check - the non empty string MUST be convertible in a ip_addr_t
		DBGNET_ASSERT( !ip_addr_t(arr_str[i]).is_null() );
		// else add the ip_addr_t to the arr_ipaddr
		arr_ipaddr.push_back(ip_addr_t(arr_str[i]));
	}
	// notify the caller with a faillure if the arr_ipaddr is empty
	if( arr_ipaddr.size() == 0 ){
		std::string reason	= "resolution failed";
		return notify_callback_err(inet_err_t(inet_err_t::ERROR, reason));
	}

	// else notify the caller with a success
	return notify_callback(inet_err_t::OK, arr_ipaddr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the host2ip_fork_t error
 */
bool	host2ip_fork_t::notify_callback_err(const inet_err_t &inet_err)		throw()
{
	return notify_callback(inet_err, std::vector<ip_addr_t>() );
}

/** \brief notify the callback with the host2ip_fork_t result
 */
bool host2ip_fork_t::notify_callback(const inet_err_t &inet_err, const std::vector<ip_addr_t> &result)	throw()
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

