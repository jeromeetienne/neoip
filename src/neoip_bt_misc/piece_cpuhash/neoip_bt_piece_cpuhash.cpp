/*! \file
    \brief Class to handle the bt_piece_cpuhash_t

*/

/* system include */
/* local include */
#include "neoip_bt_piece_cpuhash.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_io_read.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 * 
 * - it starts the operation immediatly and notify any error via the callback
 *   on a different event loop iteration.
 */
bt_piece_cpuhash_t::bt_piece_cpuhash_t(size_t piece_idx, const bt_mfile_t &bt_mfile
		, bt_io_vapi_t *bt_io_vapi, bt_piece_cpuhash_cb_t *callback, void *userptr)	throw()
{
	// sanity check - the piece_idx MUST be < to the number of 
	DBG_ASSERT( piece_idx < bt_mfile.nb_piece() );
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// launch the bt_io_read_t for it - it cant fails in the ctor
	file_range_t	totfile_range	= bt_unit_t::pieceidx_to_totfile_range(piece_idx, bt_mfile);
	bt_io_read	= bt_io_vapi->read_ctor(totfile_range, this, NULL);
}

/** \brief Destructor
 */
bt_piece_cpuhash_t::~bt_piece_cpuhash_t()		throw()
{
	// delete bt_io_read_t if needed
	nipmem_zdelete	bt_io_read;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_io_read_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_piece_cpuhash_t::neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_bt_io_read
				, const bt_err_t &io_read_err, const datum_t &read_data)	throw()
{
	bt_id_t	piecehash;
	// log to debug
	KLOG_DBG("enter io_read_err=" << io_read_err << " read_data.size()=" << read_data.size());

	// if the bt_io_read_t failed, notify the error to the caller
	if( io_read_err.failed() )	return notify_callback(io_read_err, bt_id_t());	

	// compute the piecehash over the read_data
	skey_auth_t	skey_auth("sha1/nokey/20");
	skey_auth.init_key(NULL, 0);
	skey_auth.update(read_data);
	piecehash	= bt_id_t(skey_auth.get_output());
	DBG_ASSERT( bt_id_t::size() == skey_auth.get_output().size() );

	// delete the bt_io_read_t
	nipmem_zdelete	bt_io_read;

	// notify the result to the caller
	return notify_callback(bt_err_t::OK, piecehash);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_piece_cpuhash_t::notify_callback(const bt_err_t &bt_err, const bt_id_t &piecehash)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_piece_cpuhash_cb(userptr, *this, bt_err, piecehash);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END





