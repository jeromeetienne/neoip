/*! \file
    \brief Header of the bt_http_ecnx_req_t
    
*/


#ifndef __NEOIP_BT_HTTP_ECNX_REQ_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_REQ_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_iov_arr.hpp"
#include "neoip_bt_http_ecnx_req_wikidbg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_http_ecnx_cnx_t;
class	bt_http_ecnx_iov_t;
class	bt_swarm_sched_request_t;

/** \brief class definition for bt_ecnx_req_t
 * 
 * - it is the bt_http_ecnx_cnx_t counterpart of the bt_swarm_sched_request_t
 */
class bt_http_ecnx_req_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_http_ecnx_req_t, bt_http_ecnx_req_wikidbg_init> {
private:
	bt_http_ecnx_cnx_t *		ecnx_cnx;	//!< backpointer to the bt_http_ecnx_cnx_t
	bt_swarm_sched_request_t *	sched_req;	//!< backpointer to the bt_swarm_sched_request_t
	
	bool				single_iov_mode;//!< true if this request requires several bt_iov_t
							//!< this is an optimization to avoid data copy
							//!< when there is a single bt_iov_t
	bt_iov_arr_t			iov_arr;	//!< array of the bt_iov_t not yet processed
	item_arr_t<size_t>		off_arr;	// TODO find a better name
	datum_t				recved_data;	//!< datum_t to store the temporary data
							//!< iif single_iov_mode == false
public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_req_t(bt_http_ecnx_cnx_t *ecnx_cnx, bt_swarm_sched_request_t * sched_req) 	throw();
	~bt_http_ecnx_req_t()									throw();

	/*************** query function	***************************************/
	bt_swarm_sched_request_t *	get_sched_req()		const throw()	{ return sched_req;	}
	const bt_iov_arr_t &		get_iov_arr()		const throw()	{ return iov_arr;	}	
	const bt_iov_t &		get_first_iov()		const throw()	{ return iov_arr[0];	}
	const bt_iov_t &		get_last_iov()		const throw()	{ return iov_arr[iov_arr.size()-1];}	
	const file_range_t &		orig_totfile_range()	const throw();
	const file_size_t &		orig_totfile_beg()	const throw()	{ return orig_totfile_range().beg();	}
	const file_size_t &		orig_totfile_end()	const throw()	{ return orig_totfile_range().end();	}
	file_size_t			curr_totfile_beg()	const throw();
	file_size_t			curr_totfile_end()	const throw();
	file_range_t			curr_totfile_range()	const throw();
	
	/*************** action functions	*******************************/
	bool	process_ecnx_iov(const bt_iov_t &reply_iov, const datum_t &reply_datum)	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_http_ecnx_req_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_REQ_HPP__  */



