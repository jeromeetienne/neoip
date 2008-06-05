/*! \file
    \brief Class to handle the bt_http_ecnx_req_t

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_req.hpp"
#include "neoip_bt_http_ecnx_cnx.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_ecnx_event.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
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
bt_http_ecnx_req_t::bt_http_ecnx_req_t(bt_http_ecnx_cnx_t *ecnx_cnx, bt_swarm_sched_request_t *sched_req)
										throw()
{
	bt_swarm_t *		bt_swarm	= ecnx_cnx->ecnx_pool()->get_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	file_size_t		beg_off;
	// copy the parameter
	this->ecnx_cnx	= ecnx_cnx;
	this->sched_req	= sched_req;
	
	// build the bt_iov_arr_t
	iov_arr		= bt_mfile.get_iov_arr(sched_req->totfile_range());

	// set the single_iov_mode according to the size of the iov_arr
	// - NOTE: this single_iov_mode is an optimization to avoid any memcpy if there is a single iov
	if( iov_arr.size() == 1 )	single_iov_mode	= true;
	else				single_iov_mode	= false;

	// log to debug
	KLOG_DBG("iov_arr=" << iov_arr);

	// if this request is NOT in single_iov_mode, preallocate the datum_t recved_data
	if( !single_iov_mode ){
		size_t	iov_offset = 0;
		// compute the off_arr
		// - off_arr element are the offset of this bt_iov_t within the requested range
		// - they mirror each other, if a bt_iov is removed, so should be the off_arr element
		for(size_t i = 0; i < iov_arr.size(); i++){
			// add this bt_iov_t offset 
			off_arr.append(iov_offset);
			// update the iov_offset
			iov_offset	+= iov_arr[i].length().to_size_t();
		}
		// preallocate the datum_t recved_data
		recved_data	= datum_t(iov_offset);
	}

	// link this object to bt_http_ecnx_cnx_t
	ecnx_cnx->ecnx_req_dolink(this);
}

/** \brief Destructor
 */
bt_http_ecnx_req_t::~bt_http_ecnx_req_t()		throw()
{
	// unlink this object from bt_http_ecnx_cnx_t
	ecnx_cnx->ecnx_req_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the orig_totfile_range of this request
 */
const file_range_t &	bt_http_ecnx_req_t::orig_totfile_range()	const throw()
{
	return sched_req->totfile_range();
}

/** \brief return the totfile_beg of the unprocessed part of this bt_http_ecnx_req_t
 */
file_size_t 	bt_http_ecnx_req_t::curr_totfile_beg()	const throw()
{
	bt_swarm_t *		bt_swarm	= ecnx_cnx->ecnx_pool()->get_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// sanity check - the bt_iov_arr_t MUST NOT be empty
	DBG_ASSERT( !iov_arr.empty() );
	// return the totfile_beg of first bt_iov_t
	return iov_arr[0].totfile_beg(bt_mfile);
}

/** \brief return the totfile_beg of the unprocessed part of this bt_http_ecnx_req_t
 */
file_size_t	bt_http_ecnx_req_t::curr_totfile_end()	const throw()
{
	bt_swarm_t *		bt_swarm	= ecnx_cnx->ecnx_pool()->get_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// sanity check - the bt_iov_arr_t MUST NOT be empty
	DBG_ASSERT( !iov_arr.empty() );
	// return the totfile_end of last bt_iov_t
	return iov_arr[iov_arr.size()-1].totfile_end(bt_mfile);
}

/** \brief return the totfile_range of the unprocessed part of this bt_http_ecnx_req_t
 */
file_range_t	bt_http_ecnx_req_t::curr_totfile_range()	const throw()
{
	return file_range_t(curr_totfile_beg(), curr_totfile_end());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief process the result from a bt_http_ecnx_iov_t
 * 
 * - WARNING: the returned tokeep is for the bt_http_ecnx_cnx_t!!!!!!
 *   - and not the bt_http_ecnx_req_t as usual
 */
bool	bt_http_ecnx_req_t::process_ecnx_iov(const bt_iov_t & reply_iov, const datum_t &reply_datum)	throw()
{
	// log to debug
	KLOG_DBG("enter recved_data.size()=" << reply_datum.size() );
	// sanity check - the bt_iov_arr_t MUST NOT be empty
	DBG_ASSERT( !iov_arr.empty() );

	// if the first bt_iov_t of iov_arr is within the recved range, process the first bt_iov_t
	// - NOTE: works because the ecnx_iov is notified here IFF it has matching data for 
	//   this bt_http_ecnx_req_t. and because the bt_iov_arr_t is sorted per totfile offset
	size_t	arr_idx;
	if( reply_iov.fully_include( get_first_iov() ) ){
		arr_idx	= 0;
	}else{	// else process the last bt_iov_t
		arr_idx	= iov_arr.size()-1;
	}

	// log to debug
	KLOG_DBG("arr_idx="	<< arr_idx);
	KLOG_DBG("iov_arr="	<< iov_arr);
	KLOG_DBG("reply_iov="	<< reply_iov);
	
	// sanity check - the arr_idx bt_iov_t MUST be fully included in the reply_datum
	DBG_ASSERT( reply_iov.fully_include( iov_arr[arr_idx] ) );

	// build a NOCOPY datum_t of the matching data
	file_size_t	relative_off	= iov_arr[arr_idx].subfile_beg() - reply_iov.subfile_beg();
	datum_t	 	matching_datum	= reply_datum.get_range(relative_off.to_size_t()
								, iov_arr[arr_idx].length().to_size_t()
								, datum_t::NOCOPY);

	// if this ecnx_req is in single_iov_mode, notify the data imediatly (without data copy)
	if( single_iov_mode ){
		// sanity check - the recved_data MUST NOT be initialized
		DBG_ASSERT( recved_data.is_null() );
		// notify the bt_ecnx_event_t::BLOCK_REP
		bt_ecnx_event_t	ecnx_event = bt_ecnx_event_t::build_block_rep(sched_req, &matching_datum);
		return ecnx_cnx->notify_callback( ecnx_event );
	}

	// NOTE: at this point, this bt_http_ecnx_req_t IS NOT single_iov_mode

	// sanity check - the off_arr MUST have as many element as iov_arr
	DBG_ASSERT( off_arr.size() != iov_arr.size() );
	
	// copy the reply_datum in the local datum_t recved_data
	memcpy(recved_data.ptr_off(off_arr[arr_idx]), matching_datum.void_ptr(), matching_datum.size());

	// remove the processed element in iov_arr and off_arr
	iov_arr.remove(arr_idx);
	off_arr.remove(arr_idx);
	
	// if the iov_arr is not empty, the ecnx_request has been fully processed, so notify the result
	if( iov_arr.empty() ){
		// notify the bt_ecnx_event_t::BLOCK_REP
		bt_ecnx_event_t	ecnx_event = bt_ecnx_event_t::build_block_rep(sched_req, &recved_data);
		return ecnx_cnx->notify_callback( ecnx_event );
	}

	// return tokeep - as bt_http_ecnx_cnx_t has not been deleted
	return true;
}



NEOIP_NAMESPACE_END





