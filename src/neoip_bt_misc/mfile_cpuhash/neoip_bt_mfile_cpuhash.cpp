/*! \file
    \brief Class to handle the bt_mfile_cpuhash_t

\par Brief Description
\ref bt_mfile_cpuhash_t regenerates all the pieces of a bt_mfile_t based on the
local files. Obvious it assumes they are all available. It could used:
-# when generation a bt_mfile_t in order to publish it

*/

/* system include */
/* local include */
#include "neoip_bt_mfile_cpuhash.hpp"
#include "neoip_bt_piece_cpuhash.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_mfile_cpuhash_t::bt_mfile_cpuhash_t()		throw()
{
	// zero some field
	piece_cpuhash	= NULL;
}

/** \brief Destructor
 */
bt_mfile_cpuhash_t::~bt_mfile_cpuhash_t()		throw()
{
	// delete bt_piece_cpuhash_t if needed
	nipmem_zdelete	piece_cpuhash;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_mfile_cpuhash_t::start(const bt_mfile_t &bt_mfile, bt_io_vapi_t *bt_io_vapi
				, bt_mfile_cpuhash_cb_t *callback, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// sanity check - the bt_mfile_t MUST NOT be null
	DBG_ASSERT( !bt_mfile.is_null() );

	// copy the parameter
	this->bt_mfile	= bt_mfile;
	this->bt_io_vapi= bt_io_vapi;
	this->callback	= callback;
	this->userptr	= userptr;
	
	// reset the piecehash_arr before launching the first bt_piece_cpuhash_t
	piecehash_arr	= bt_id_arr_t();
	
	// launch the first bt_piece_cpuhash_t
	launch_next_piece();

	// return no error
	return bt_err_t::OK;
}
 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      launch the next bt_piece_cpuhash_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_piece_cpuhash_t
 */
void	bt_mfile_cpuhash_t::launch_next_piece()	throw()
{
	size_t		piece_idx	= piecehash_arr.size();
	// sanity check - bt_piece_cpuhash_t MUST NOT be running
	DBG_ASSERT( !piece_cpuhash );
	// launch the bt_piece_cpuhash_t for it
	piece_cpuhash	= nipmem_new bt_piece_cpuhash_t(piece_idx, bt_mfile, bt_io_vapi, this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_piece_cpuhash_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_mfile_cpuhash_t::neoip_bt_piece_cpuhash_cb(void *cb_userptr, bt_piece_cpuhash_t &cb_piece_cpuhash
				, const bt_err_t &cpuhash_err, const bt_id_t &piecehash)	throw()
{
	// log to debug
	KLOG_WARN("enter piece_cpuhash_err=" << cpuhash_err << " piecehash=" << piecehash);

	// if the bt_piece_cpuhash_t failed, notify the error to the caller
	if( cpuhash_err.failed() )	return notify_callback(cpuhash_err, bt_id_arr_t());	

	// add the piecehash to the local piecehash_arr
	piecehash_arr	+= piecehash;
	
	// delete the bt_piece_cpuhash_t
	nipmem_zdelete	piece_cpuhash;
	
	// if all the needed piece hash have been already computed, notify completion to the caller
	if( piecehash_arr.size() == bt_mfile.nb_piece() ){
		// it is copied in stack, thus the notified object may delete the notifier 
		// before using the notified variable
		bt_id_arr_t	piecehash_arr_copy(piecehash_arr);
		// notify the caller
		return notify_callback(bt_err_t::OK, piecehash_arr_copy);
	}
	
	// launch the first bt_piece_cpuhash_t
	launch_next_piece();

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_mfile_cpuhash_t::notify_callback(const bt_err_t &bt_err, const bt_id_arr_t &piecehash_arr) throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_mfile_cpuhash_cb(userptr, *this, bt_err, piecehash_arr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





