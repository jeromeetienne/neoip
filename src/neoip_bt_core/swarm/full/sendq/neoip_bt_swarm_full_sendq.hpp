/*! \file
    \brief Header of the bt_swarm_full_t
    
*/


#ifndef __NEOIP_BT_SWARM_FULL_SENDQ_HPP__ 
#define __NEOIP_BT_SWARM_FULL_SENDQ_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_swarm_full_sendq_wikidbg.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_cmd.hpp"
#include "neoip_bt_io_read_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for bt_swarm_full_sendq_t
 */
class bt_swarm_full_sendq_t : NEOIP_COPY_CTOR_DENY
			, private bt_io_read_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<bt_swarm_full_sendq_t, bt_swarm_full_sendq_wikidbg_init>
			{
private:
	bt_swarm_full_t *	swarm_full;	//!< the bt_swarm_full_t to which it is attached
	std::list<bt_cmd_t>	cmd_queue;	//!< the current bt_cmd_t queue
	
	/*************** Internal function	*******************************/
	void		fill_socket_xmitbuf()	throw();
	bool		send_front_cmd()	throw();

	/*************** bt_io_read_t	***************************************/
	bt_io_read_t *	bt_io_read;
	bool 		neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_io_read
					, const bt_err_t &bt_err, const datum_t &read_data)	throw();	

	/*************** xmit_kalive_timeout	*******************************/
	timeout_t	xmit_kalive_timeout;	//!< triggered when the local peer must send a keepalive
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();	
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_full_sendq_t(bt_swarm_full_t *swarm_full) 		throw();
	~bt_swarm_full_sendq_t()					throw();

	/*************** query function	***************************************/
	size_t		nb_queued_cmd()		const throw() { return cmd_queue.size();}	
	
	/*************** action function	*******************************/
	void		queue_cmd(const bt_cmd_t &bt_cmd)		throw();
	void		notify_maysend()				throw();
	void		remove_one_block_rep(const bt_cmd_t &bt_cmd)	throw();
	void		remove_all_block_rep()				throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_swarm_full_sendq_wikidbg_t;
	friend class 	bt_swarm_full_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_FULL_SENDQ_HPP__  */



