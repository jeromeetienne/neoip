/*! \file
    \brief Definition of the \ref agethostbyname_t class

- TODO in order to avoid thread and their cost even when unused, i can fork
  - this modules use thread, and thread consume a lot of resource even
    when unused because they keep locking shared resource even when there
    is only one thread
  - porting asyncop_t to fork would solve this issue.
    - it would make the initialization of the asyncop_t much longer but would
      reduce its cost to the period where it is used.
    - thus using fork would be a gain if the asyncop_t is unfrequent.
  - in this case, should i fork on my own process, aka a large one or forking
    an external tool to do the gethostbyname for me ?

*/

/* system include */
#include <netdb.h>
#include <sys/socket.h>
/* local include */
#include "neoip_agethostbyname.hpp"
#include "neoip_asyncop.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Callback notified to do the work
 * 
 * - IMPORTANT: this function is NOT running in the 'main' thread
 * 
 * @return a pointer on the result if one is provided, NULL otherwise
 */
static void *agethostbyname_do_work_cb(void *userptr)	throw() 
{
	char *		hostname	= (char *)userptr;
	char		buf[10*1024];
	struct hostent	hostbuf, *entry;
	int		herr, res;
	// log to debug
	KLOG_DBG("enter");
	// call the gethostbyname() which is thread-ok
	res = gethostbyname_r(hostname, &hostbuf, buf, sizeof(buf), &entry, &herr);
	// handle error case
	if( res || entry == NULL ){
		KLOG_DBG("unable to resolve");
		// return a null hostent_t
		return new hostent_t();
	}
	// retun the hostent_t pointer as work_result
	return new hostent_t(entry);
}

/** \brief Callback notified to free a potential the work
 * 
 * - IMPORTANT: this function IS running in the 'main' thread
 */
static void agethostbyname_free_work_cb(void *work_result, void *userptr)	throw() 
{
	// log to debug
	KLOG_DBG("enter work_result=" << work_result << " userptr=" << userptr);
	// delete the hostent
	delete (hostent_t *)work_result;
	// free the hostname in the userptr
	free( userptr );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
agethostbyname_t::agethostbyname_t()	throw()
{
	// zero some fields
	asyncop	= NULL;
}

/** \brief Destructor
 */
agethostbyname_t::~agethostbyname_t()	throw()
{
	// delete the asyncop_t if needed
	nipmem_zdelete asyncop;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	agethostbyname_t::start(const std::string &hostname, agethostbyname_cb_t *callback
							, void *userptr)	throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	
#ifdef USE_ELOOP_LEVT	// TODO to remove - it is there because currently it is not supported
	DBG_ASSERT( 0 );
#endif

	// create the asyncop_t
	bool		failed;
	asyncop		= nipmem_new asyncop_t();
	failed		= asyncop->start(this, strdup(hostname.c_str()), agethostbyname_do_work_cb
							, agethostbyname_free_work_cb);
	if( failed )	return inet_err_t(inet_err_t::ERROR, "asyncop failed to start");

	// return no error
	return inet_err_t::OK;
}


/** \brief asyncop callback notified when the operation is completed
 */
void agethostbyname_t::neoip_asyncop_completed_cb(void *cb_userptr, asyncop_t &asyncop
							, void *work_result)	throw()
{
	hostent_t *	hostent = (hostent_t *)work_result;
	// log to debug	
	KLOG_DBG("enter hostent=" << hostent);
	// notify the result to the caller
	callback->neoip_agethostbyname_cb(userptr, *this, *hostent);
}

NEOIP_NAMESPACE_END

