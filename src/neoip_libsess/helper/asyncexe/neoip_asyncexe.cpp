/*! \file
    \brief Definition of the \ref asyncexe_t class

- 3 fdwatch_t
  - one fdwatch_in to fill the input of the exec
  - one fdwatch_out to get the output of the exec
  - one fdwatch_err to get the error of the exec
- start(item_arr_t<string> &cmdline_arr, const datum_t &std_input, callback, userptr)
  - what about the error returned ? a libsess_err_t ?
  - i need one, i dunno which one, put libsess_err_t and see later
- nunit stdin("bonjour") prog(wc -l) expected anwer == 7
  - test with file not found too
- the stderr is not handled by the pipe()
  - hmm currently ignore the stderr from the exec

\par Note about stderr/stdin and pipe()
- as pipe() syscall doesnt handle the stdin/stderr of the launch exec, it is 
  currently ignored by the asyncexe_t
- thus if the exec sends data to stderr, they will be silently discarded
  - NOTE: the exec may even block as it can't send data
- POSSIBLE IMPROVEMENT:
  - dont use pipe() directly but your own implementation with a stdin/stderr handling
    - but not needed now and i want to go fast

*/

/* system include */
#include <fcntl.h>
#ifndef _WIN32
#	include <sys/wait.h>
#endif
/* local include */
#include "neoip_asyncexe.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
asyncexe_t::asyncexe_t()	throw()
{
	// zero some field
	stdout_fdwatch	= NULL;
}

/** \brief Destructor
 */
asyncexe_t::~asyncexe_t()	throw()
{
	// delete the stdout_fdwatch if needed
	nipmem_zdelete	stdout_fdwatch;
	
#ifndef _WIN32
#if 1	// TODO: BUG: there is a LOT of defunct of neoip-dnsresolve while using neoip-webpack
	// - this is a very early experiment of this issue
	// - POSSIBLE SOLUTION:
	//   - when a waitpid is done, just after childpid is set to 0
	//   - in dtor, if childpip != 0, do a non blocking waitpid on it
	// - ANOTHER SOLUTION:
	//   - similar but do it on ALL childpid
	//   - it would be more robust
	//   - it may cause to report inproper status. 
	//   - not that big of a trouble....
	// - do experimentation and decide
	// do a non-blocking waitpid on all possible children
	while( waitpid(-1, NULL, WNOHANG) > 0 );
#endif
#endif	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
asyncexe_t &asyncexe_t::set_profile(const asyncexe_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == libsess_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief Start the operation
 * 
 * @return true on error, false otherwise
 */
libsess_err_t	asyncexe_t::start(const item_arr_t<std::string> &m_cmdline_arr
					, asyncexe_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->m_cmdline_arr	= m_cmdline_arr;
	this->callback		= callback;
	this->userptr		= userptr;

	// start the expire_ttimeout
	expire_timeout.start(profile.expire_delay(), this, NULL);

	// launch the zerotimer_t to perform the action in the next iteration of the event loop
	zerotimer.append(this, NULL);	

#ifdef _WIN32
	EXP_ASSERT(0);
#endif
	// return no error
	return libsess_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool asyncexe_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// notify the expiration to the caller
	std::string	reason	= "asyncexe_t timeout after " + OSTREAMSTR(expire_timeout.get_period());
	return notify_callback_err(libsess_err_t(libsess_err_t::ERROR, reason));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			zerotimer_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	asyncexe_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
#ifndef	_WIN32
	int	pipefrom[2];    // pipe to get the executable output
	int	res;
#endif
	// log to debug
	KLOG_DBG("enter");

	// convert the cmdline_arr() into a argv[] format
	char *	argv[64];
	DBG_ASSERT( cmdline_arr().size() < sizeof(argv)/sizeof(argv[0]));
	for( size_t i = 0; i < cmdline_arr().size(); i++ ){
		const std::string &	arg	= cmdline_arr()[i];
		// allocate the memory - in the stack
		argv[i]	= (char *)nipmem_alloca(arg.size() + 1);
		// copy the data
		memcpy(argv[i], arg.c_str(), arg.size() + 1 ); 
	}
	// put a NULL at the end of the array
	argv[cmdline_arr().size()]	= NULL;
	
#ifndef	_WIN32
	// create the pipe fd's
	res	= pipe( pipefrom );
	if( res != 0 ){
		std::string reason	= "failed pipe() due to " + neoip_strerror(errno);
		return notify_callback_err(libsess_err_t(libsess_err_t::ERROR, reason));
	}

	// fork the execution
	childpid= fork(); 
	if( childpid == -1 ){
		std::string reason	= "failed fork() due to " + neoip_strerror(errno);
		return notify_callback_err(libsess_err_t(libsess_err_t::ERROR, reason));
	}
	
	// if it is the child running here, do the child processing
	if( childpid == 0 ){
		// set the stdout fd to the pipe one
		dup2( pipefrom[1], STDOUT_FILENO  );
		// close the pipefrom fd's - the STDOUT_FILENO remain open tho
		close( pipefrom[0] );
		close( pipefrom[1] );
		// go thru the whole fd table to close all fd except the STDOUT_FILENO or STDERR_FILENO
		// - NOTE: this seems rather dirty but it is the less worst alternative due to :
		//   - the other alternative is to set the 'close-on-exec' flag on all other fd
		//     opened anywhere in the code. via the fnctl() F_SETFD
		//   - this would spread this special case on the whole code
		//   - getdtablesize() defaults to 1024 on linux (so rather small)
		//   - getdtablesize() is well contained in here, so match the locality rule
		// - the STDERR_FILENO is kept only to be able to log error to stderr if execl() fails
		size_t	nb_fd_max	= getdtablesize();
		for(size_t cur_fd = 0; cur_fd < nb_fd_max; cur_fd++ ){
			// if cur_fd is STDOUT_FILENO or STDERR_FILENO, goto the next
			if( cur_fd == STDOUT_FILENO || cur_fd == STDERR_FILENO )	continue;
			// close the cur_fd
			close(cur_fd);
		}
		// launch the external program now
		execvp(argv[0], argv);
		// if this point is reached, the execl() failed, log the event
		// - STDERR_FILENO has been left open just for that
		KLOG_STDERR("Cant exec " << argv[0] << " due to " << neoip_strerror(errno) << "\n");
		// report -1 as error to the parent process
		exit(-1);
	}
	// NOTE: here only the parent is running
	DBG_ASSERT( childpid != 0 );

	// close the unneeded fd
	close(pipefrom[1]);

	/*************** init stdout_fdwatch	*******************************/
	// set this pipefrom[0] in non blocking
	res	= fcntl(pipefrom[0], F_SETFL, O_NONBLOCK);
	if( res != 0 ){
		std::string reason	= "failed fcntl() O_NONBLOCK due to " + neoip_strerror(errno);
		return notify_callback_err(libsess_err_t(libsess_err_t::ERROR, reason));
	}
	// start the stdout_fdwatch
	stdout_fdwatch = nipmem_new fdwatch_t();
	stdout_fdwatch->start(pipefrom[0], fdwatch_t::INPUT, this, NULL);
#endif

	// return tokeep
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			fdwatch_cb_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified with fdwatch_t has an condition to notify
 */
bool	asyncexe_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &fdwatch_cond )	throw()
{
	// log the event
	KLOG_DBG("enter fdwatch_cond=" << fdwatch_cond);
	// forward to the proper callback
	if( stdout_fdwatch == &cb_fdwatch )	return stdout_fdwatch_cb(cb_userptr, cb_fdwatch, fdwatch_cond);
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			stdout_fdwatch callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified with fdwatch_t has an condition to notify
 */
bool	asyncexe_t::stdout_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &fdwatch_cond )	throw()
{
	int	readlen		= 0;	
	// log the event
	KLOG_DBG("enter fdwatch_cond=" << fdwatch_cond);

	// if the condition is input
	if( fdwatch_cond.is_input() ){
		size_t	recv_max_len	= 16*1024;
		void *	buffer		= nipmem_alloca(recv_max_len);
		// read data in the socket
		readlen = read(stdout_fdwatch->get_fd(), buffer, recv_max_len);
		// if readlen < 0, treat it as error
		// - due to a weird bug in glib|linux, G_IO_ERR/HUP isnt received
		//   so there is a test if (cond.is_input() && readlen==0) fallthru
		//   and treat it as error
		if( readlen < 0 )	readlen = 0;
		// if some data have been read, add them to the stdout_barray
		if( readlen > 0 )	stdout_barray.append(buffer, readlen);
		// log to debug
		KLOG_DBG("readlen=" << readlen);
	}

	// handle a connection error
	if( fdwatch_cond.is_error() || (fdwatch_cond.is_input() && readlen == 0) ){
		// wait for the pid to get the result
		int	status	= 0;
#ifndef _WIN32
		int	ret	= waitpid(childpid, &status, 0);
		if( ret == -1 && errno == ECHILD )	status = 0;
		else if( ret != childpid )		status = -1;
		// extract the return status
		status		= WEXITSTATUS(status);
#endif
		// log to debug
		KLOG_ERR("childpid=" << childpid << " return status=" << status );
		KLOG_ERR("received error. now recved_data=" << stdout_barray.to_datum() );
		// else notify the caller with a success
		return notify_callback(libsess_err_t::OK, stdout_barray, status);
	}

	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the asyncexe_t result
 */
bool asyncexe_t::notify_callback_err(const libsess_err_t &libsess_err)	throw()
{
	return notify_callback(libsess_err, bytearray_t(), -1);	
}

/** \brief notify the callback with the asyncexe_t result
 */
bool asyncexe_t::notify_callback(const libsess_err_t &libsess_err, const bytearray_t &stdout_barray
						, const int &exit_status)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_asyncexe_cb(userptr, *this, libsess_err, stdout_barray, exit_status);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END

