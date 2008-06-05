/*! \file
    \brief Definition of the \ref asyncexe_t class
    
*/

/* system include */
#include <netdb.h>
#include <sys/socket.h>
/* local include */
#include "neoip_asyncexe.hpp"
#include "neoip_asyncop.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//! contain the result of the exe - used only INSIDE this module only
struct asyncexe_work_result_t {
	std::string	launch_error;	//!< contain the text of a error produced while trying to launch
					//!< the exe. if .empty() == true, no error occured
	std::string	std_output;	//!< contain the standard output result of the execution
	std::string	std_error;	//!< contain the standard error result of the execution
	int		exit_status;	//!< the exit_status returned by the execution
};

/** \brief Callback notified to do the work
 * 
 * - IMPORTANT: this function is NOT running in the 'main' thread
 * 
 * @return a pointer on the result if one is provided, NULL otherwise
 */
static void *asyncexe_do_work_cb(void *userptr)	throw() 
{
	gchar *		cmd_line	= (gchar *)userptr;
	KLOG_ERR("cmd_line=" << cmd_line);

	gchar *	std_output;
	gchar *	std_error;
	gint 	exit_status;
	GError *gerror	= NULL;
	gboolean	succeed;
	
	// launch the command line in sync
	succeed = g_spawn_command_line_sync(cmd_line, &std_output, &std_error, &exit_status, &gerror);

	// allocate the work_result to return
	asyncexe_work_result_t *work_result = new asyncexe_work_result_t();

	// if an error occurs while launching the command - aka not in the execution of the command
	if( !succeed || gerror != NULL ){
		// sanity check
		DBG_ASSERT( !succeed );
		DBG_ASSERT( gerror );
		// copy the gerror message
		work_result->launch_error	= gerror->message;
		work_result->exit_status	= -1;
		// free the gerror
		g_error_free(gerror);
	}else{
		// convert the result in a asyncexe_work_result_t
		work_result->std_output	= std_output;
		work_result->std_error	= std_error;
		work_result->exit_status= exit_status;
	}
	
	// free the std_output, std_error returned by the glib function
	g_free(std_output);
	g_free(std_error);
	
	// return the work result
	return work_result;
}

/** \brief Callback notified to free a potential the work
 * 
 * - IMPORTANT: this function IS running in the 'main' thread
 */
static void asyncexe_free_work_cb(void *work_result, void *userptr)	throw() 
{
	// log to debug
	KLOG_DBG("enter work_result=" << work_result << " userptr=" << userptr);
	// delete the work_result
	delete (asyncexe_work_result_t *)work_result;
	// free the command line in the userptr
	free( userptr );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
asyncexe_t::asyncexe_t()	throw()
{
	asyncop	= NULL;
}

/** \brief Destructor
 */
asyncexe_t::~asyncexe_t()	throw()
{
	// delete the asyncop_t if needed
	if( asyncop )	nipmem_delete asyncop;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * @return true on error, false otherwise
 */
bool	asyncexe_t::start(const std::string &cmd_line, asyncexe_cb_t *callback
							, void *userptr)	throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;

	// create the asyncop_t
	asyncop	= nipmem_new asyncop_t();
	bool	failed = asyncop->start(this, strdup(cmd_line.c_str()), asyncexe_do_work_cb
							, asyncexe_free_work_cb);
	if( failed ){
		KLOG_ERR("Can't start asyncop_t for command line " << cmd_line);
		return true;
	}

	// return no error
	return false;
}


/** \brief asyncop callback notified when the 
 */
void asyncexe_t::neoip_asyncop_completed_cb(void *cb_userptr, asyncop_t &asyncop
							, void *work_result)	throw()
{
	asyncexe_work_result_t *	result = (asyncexe_work_result_t *)work_result;
	// notify the result to the caller
	callback->neoip_asyncexe_cb(userptr, *this, result->launch_error, result->std_output
						, result->std_error, result->exit_status);
}

NEOIP_NAMESPACE_END

