/*! \file
    \brief Class to handle the bt_check_t

\par Brief Description
\ref bt_check_t allows to check the files contents of a bt_mfile_t.
bt_swarm_resumedata_t may be available or not. It is determined depending
on the used start() function, with bt_swarm_resumedata_t xor with bt_mfile_t.
- If the bt_swarm_resumedata_t IS available, this means the files have been
  used by a previous bt_swarm_t. In this case, the bt_mfile_t from the 
  bt_swarm_resumedata_t is used.
- If the bt_swarm_resumedata_t IS NOT available, this means the files have
  been created by an external mean, e.g. another bt client.
  
\par About result notification
- if the notified bt_swarm_resumedata_t is not null, it can immediatly be
  used to start bt_swarm_t.
- if an error occurs, it is notified in the bt_err_t
  - a non-ok bt_err_t means the result is not the expected one
  - NOTE: notifying a non-ok bt_err_t and a non-null bt_swarm_resumedata_t
    is possible, and the bt_swarm_resumedata_t may still be used on a bt_swarm_t

\par About the policy
\ref bt_check_policy_t allows 3 policies
- bt_check_policy_t::SUBFILE_EXIST
  - it it done IIF the bt_io_vapi_t is bt_io_sfile_t, else it is ALWAYS succeeding
  - SUBFILE_EXIST may be used when starting the bt_check_t with bt_swarm_resumedata_t AND bt_mfile_t
  - for each subfile, it check it is accessible in read/write and check that the file
    length matches the one from the bt_mfile_t
- bt_check_policy_t::PARTIAL_PIECE
  - PARTIAL_PIECE may be used IIF the bt_check_t is started with a bt_swarm_resumedata_t
  - It performes the same check as SUBFILE_EXIST and additionnally checks the hash of the
    pieces which were partial when the bt_swarm_resumedata_t have been saved.
- bt_check_policy_t::EVERY_PIECE
  - EVERY_PIECE may be used when starting the bt_check_t with bt_swarm_resumedata_t AND bt_mfile_t
  - It performes the same check as SUBFILE_EXIST and additionnal check the hash of all
    the pieces of the bt_mfile_t
    - typically usefull when checking file created by an external mean (aka started with bt_mfile_t)
    - it may be used for ultra safety when files have been created by a bt_swarm_t
      but the caller would like to check if they havent been modified after bt_swarm_t ended

*/

/* system include */
/* local include */
#include "neoip_bt_check.hpp"
#include "neoip_bt_piece_cpuhash.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_bt_io_sfile.hpp"	// just to check in SUBFILE_EXIST

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_check_t::bt_check_t()		throw()
{
	// zero some field
	piece_cpuhash	= NULL;	
}

/** \brief Destructor
 */
bt_check_t::~bt_check_t()		throw()
{
	// destruct the bt_piece_cpuhash_t
	nipmem_zdelete	piece_cpuhash;	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_check_t::internal_start(const bt_check_policy_t &check_policy
				, const bt_mfile_t &bt_mfile
				, const bt_swarm_resumedata_t &caller_resumedata
				, bt_io_vapi_t *bt_io_vapi
				, bt_check_cb_t *callback, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_ERR("enter");
	// sanity check - bt_mfile_t and bt_swarm_resume_data MUST NOT have the same is_null() status
	DBG_ASSERT( !bt_mfile.is_null() );
	// sanity check - the bt_check_policy_t MUST NOT be null
	DBG_ASSERT( !check_policy.is_null() );

	// copy the parameter
	this->check_policy	= check_policy;
	this->bt_mfile		= bt_mfile;
	this->caller_resumedata	= caller_resumedata;
	this->bt_io_vapi	= bt_io_vapi;
	this->callback		= callback;
	this->userptr		= userptr;

	// start the zerotimer_t to start the checking asyncronously
	// - this avoid any nested callback issue
	zerotimer.append(this, NULL);
	
	// return no error
	return bt_err_t::OK;
}


/** \brief Start the operation from a fully init bt_mfile_t
 * 
 * - this function MUST be used IF there is a bt_swarm_resumedata_t available
 *   - the bt_mfile_t is taken from bt_swarm_resumedata_t
 * - just a helper on top of internal_start()
 */
bt_err_t	bt_check_t::start(const bt_check_policy_t &check_policy
				, const bt_swarm_resumedata_t &caller_resumedata, bt_io_vapi_t *bt_io_vapi
				, bt_check_cb_t *callback, void *userptr)	 throw()
{
	// sanity check - the bt_swarm_resumedata_t MUST be check ok
	DBG_ASSERT( caller_resumedata.check().succeed() );
	// call the internal start
	return internal_start(check_policy, caller_resumedata.bt_mfile(), caller_resumedata, bt_io_vapi, callback, userptr);
}

/** \brief Start the operation
 * 
 * - this function MUST bt used IIF there is no bt_swarm_resumedata_t available
 * - just a helper on top of internal_start()
 */
bt_err_t	bt_check_t::start(const bt_check_policy_t &check_policy, const bt_mfile_t &bt_mfile
			, bt_io_vapi_t *bt_io_vapi, bt_check_cb_t *callback, void *userptr) throw()
{
	// sanity check - the bt_mfile_t MUST be fully init
	DBG_ASSERT( bt_mfile.is_fully_init() );
	// sanity check - the check_policy MUST NOT be PARTIAL_PIECE
	DBG_ASSERT( check_policy != bt_check_policy_t::PARTIAL_PIECE);
	// call the internal start
	return internal_start(check_policy, bt_mfile, bt_swarm_resumedata_t(), bt_io_vapi, callback, userptr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_check_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// check the file access
	bt_err		= check_all_file_access();
	if( bt_err.failed() )	return notify_callback_err(bt_err);

	// if it is in bt_check_policy_t::SUBFILE_EXIST, notify the result now
	// - NOTE: as the piecehash_arr is not recomputed, the bt_swarm_resumedata_t can not
	//   be regenerated, and it replies the one from the caller (even if it is null)
	if( check_policy == bt_check_policy_t::SUBFILE_EXIST )
		return notify_callback(bt_err_t::OK, caller_resumedata);

	// sanity check - the bt_mfile.do_piecehash() MUST be true
	DBG_ASSERT( bt_mfile.do_piecehash() );

 	// populate the tocheck_bfield depending on the policy
 	if( check_policy == bt_check_policy_t::PARTIAL_PIECE )	tocheck_bfield_ctor_partial();
 	else							tocheck_bfield_ctor_every();

	// init a empty isavail_bfield
	isavail_bfield	= bitfield_t(bt_mfile.nb_piece());

	// goto the first pieceidx to check
	cur_pieceidx	= tocheck_bfield.get_next_set(0);

	// launch the first bt_piece_cpuhash_t
	bool	tokeep	= launch_piece_cpuhash();
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         file access checking
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Check if all local files may be accessed
 */
bt_err_t	bt_check_t::check_all_file_access()				throw()
{
	const bt_mfile_subfile_arr_t &	subfile_arr	= bt_mfile.subfile_arr();
	bt_err_t			bt_err;
	// if the bt_io_vapi_t is NOT a bt_io_sfile_t, return OK now
	// - NOTE: this check is done IIF bt_io_vapi_t is bt_io_sfile_t
	if(dynamic_cast<bt_io_sfile_t *>(bt_io_vapi) == NULL)	return bt_err_t::OK;
	// go thru all the local files in the bt_mfile_t
	for(size_t i = 0; i < subfile_arr.size(); i++){
		bt_err	= check_one_file_access(subfile_arr[i]);
		if( bt_err.failed() )	return bt_err;
	}		
	// return no error
	return bt_err_t::OK;
}

/** \brief Check if this file may be accessed
 * 
 * - aka open in RW and having a length matching with bt_mfile_t
 */
bt_err_t	bt_check_t::check_one_file_access(const bt_mfile_subfile_t &mfile_subfile)	throw()
{
	file_stat_t	file_stat;
	file_err_t	file_err;
	// log to debug
	KLOG_ERR("check mfile_subfile=" << mfile_subfile);
	// test if the file exists
	file_err	= file_stat.open(mfile_subfile.local_path());
	if( file_err.failed() )	return bt_err_from_file(file_err);

	// test if the actual length of the file matches the one from bt_mfile_subfile_t
	if( file_stat.get_size() != mfile_subfile.len() ){
		std::ostringstream	reason;
		reason << mfile_subfile.local_path() << " is " << file_stat.get_size()
				<< "-byte long but should be " << mfile_subfile.len() << "-byte long.";
		return bt_err_t(bt_err_t::ERROR, reason.str());
	}

	// test if the file may be open in file_mode_t:RW
	file_sio_t	file_sio;
	file_err	= file_sio.start(mfile_subfile.local_path(), file_mode_t::RW);
	if( file_err.failed() )	return bt_err_from_file(file_err);

	// return no error - as all tests passed
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the tocheck_bfield with the PARTIAL piece
 */
void	bt_check_t::tocheck_bfield_ctor_partial()	throw()
{
	// sanity check - the caller_resumedata MUST NOT be null
	DBG_ASSERT( !caller_resumedata.is_null() );
	// get a tocheck_bfield from the partavail_piece_inval in the bt_swarm_resumedata_t
	tocheck_bfield	= caller_resumedata.get_partavail_piece_bitfield();
	// sanity check - the tocheck_bfield MUST have as many bit as the bt_mfile.nb_piece()
	DBG_ASSERT( tocheck_bfield.size() == bt_mfile.nb_piece() );
}

/** \brief Populate the tocheck_bfield with the EVERY piece
 */
void	bt_check_t::tocheck_bfield_ctor_every()	throw()
{
	// init the tocheck_bfield
	tocheck_bfield	= bitfield_t(bt_mfile.nb_piece());
	// set it all to one
	for(size_t piece_idx = 0; piece_idx < tocheck_bfield.size(); piece_idx++)
		tocheck_bfield.set(piece_idx, true);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the next bt_piece_cpuhash_t
 * 
 * @return a tokeep for the bt_check_t
 */
bool	bt_check_t::launch_piece_cpuhash()	throw()
{
	// sanity check - the piece_cpuhash MUST be null
	DBG_ASSERT( !piece_cpuhash );
	
	// if cur_pieceidx has not reach the end, launch the bt_piece_cpuhash_t for cur_pieceidx
	if( cur_pieceidx != tocheck_bfield.size() ){
		piece_cpuhash	= nipmem_new bt_piece_cpuhash_t(cur_pieceidx, bt_mfile, bt_io_vapi, this, NULL);
		// return tokeep
		return true;
	}
	
	// NOTE: here the all the piece to be check have been checked, notify the result to the caller 
	
	bt_err_t		bt_err	= bt_err_t::OK;
	bt_swarm_resumedata_t	result_resumedata;
	// a non-ok bt_err_t is return IIF a piece_idx was supposed to be available but is not
	// - if check_policy is EVERY_PIECE
	// - if a swarm_resumedata has been provided by the caller
	// - if this caller resumedata declare piece available but are not
	if( check_policy == bt_check_policy_t::EVERY_PIECE && !caller_resumedata.is_null()
			&& (caller_resumedata.pieceavail_local() ^ isavail_bfield).is_any_set() ){
		std::string	reason	= "piece declared available in swarm_resumdata but is not in practice";
		bt_err	= bt_err_t(bt_err_t::ERROR, reason);
	}
	// if the caller_resumedata is not null, copy it from the caller, else build a minimal one
	if( !caller_resumedata.is_null() )	result_resumedata	= caller_resumedata;
	else					result_resumedata.bt_mfile(bt_mfile);
	// set the pieceavail_local in the result_resumedata
	if( check_policy == bt_check_policy_t::EVERY_PIECE ){
		// replace the result_resumedata.pieceavail_local by the isavail_bfield
		result_resumedata.pieceavail_local( bt_pieceavail_t(isavail_bfield) );
	}else{
		// sanity check - 
		DBG_ASSERT( !result_resumedata.pieceavail_local().is_null() );
		DBG_ASSERT( check_policy == bt_check_policy_t::PARTIAL_PIECE );
		// set the result_resumedata.pieceavail_local by the boolean or of the previous one and 
		// the isavail_bfield
		bitfield_t	pieceavail_sum	= isavail_bfield | result_resumedata.pieceavail_local();
		result_resumedata.pieceavail_local( bt_pieceavail_t(pieceavail_sum) );
	}

	// notify the caller
	return notify_callback(bt_err, result_resumedata);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_piece_cpuhash_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_check_t::neoip_bt_piece_cpuhash_cb(void *cb_userptr, bt_piece_cpuhash_t &cb_piece_cpuhash
				, const bt_err_t &cpuhash_err, const bt_id_t &piecehash)	throw()
{
	// log to debug
	KLOG_WARN("enter piece_cpuhash_err=" << cpuhash_err << " piecehash=" << piecehash);

	// if the bt_piece_cpuhash_t failed, notify the error to the caller
	if( cpuhash_err.failed() )	return notify_callback_err(cpuhash_err);

	// if the computed piecehash matches the one from the bt_mfile_t, set the bit in isavail_bfield
	if( piecehash == bt_mfile.piecehash_arr()[cur_pieceidx])
		isavail_bfield.set(cur_pieceidx, true);

	// goto the next cur_pieceidx to check
	cur_pieceidx	= tocheck_bfield.get_next_set(cur_pieceidx);

	// launch the first bt_piece_cpuhash_t
	bool	tokeep	= launch_piece_cpuhash();
	if( !tokeep )	return false;

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
bool bt_check_t::notify_callback(const bt_err_t &bt_err, const bt_swarm_resumedata_t &swarm_resumedata) throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_check_cb(userptr, *this, bt_err, swarm_resumedata);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





