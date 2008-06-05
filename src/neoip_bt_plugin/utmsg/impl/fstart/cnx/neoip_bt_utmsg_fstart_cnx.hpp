/*! \file
    \brief Header of the bt_utmsg_fstart_cnx_t
    
*/


#ifndef __NEOIP_BT_UTMSG_FSTART_CNX_HPP__ 
#define __NEOIP_BT_UTMSG_FSTART_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_utmsg_fstart_cnx_wikidbg.hpp"
#include "neoip_bt_utmsg_cnx_vapi.hpp"
#include "neoip_bt_utmsg_fstart.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_full_utmsg_t;
class	bt_cmd_t;
class	pkt_t;
class	bt_utmsgtype_t;

/** \brief class definition for bt_utmsg_fstart_cnx_t
 */
class bt_utmsg_fstart_cnx_t : NEOIP_COPY_CTOR_DENY, public bt_utmsg_cnx_vapi_t
		, private wikidbg_obj_t<bt_utmsg_fstart_cnx_t, bt_utmsg_fstart_cnx_wikidbg_init, bt_utmsg_cnx_vapi_t>
		{
private:
	bt_utmsg_fstart_t *	utmsg_fstart;	//!< backpointer on the bt_utmsg_fstart_t
	bt_swarm_full_utmsg_t *	full_utmsg;	//!< backpointer on the bt_swarm_full_utmsg_t
	bool			m_remote_fstart;//!< the faststart state for the remote peer
	
	/*************** internal function	*******************************/
	bool			parsing_error(const std::string &reason)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_fstart_cnx_t() 		throw();
	~bt_utmsg_fstart_cnx_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(bt_utmsg_fstart_t *utmsg_fstart
					, bt_swarm_full_utmsg_t *m_full_utmsg)	throw();	

	/*************** Query function	***************************************/
	bool		remote_fstart()	const throw()	{ return m_remote_fstart;	}

	/*************** action function	*******************************/
	void		send_cmd(const bt_cmd_t &bt_cmd)	const throw();

	/*************** bt_utmsg_cnx_vapi_t	*******************************/
	bool		parse_pkt(const bt_utmsgtype_t &bt_utmsgtype, const pkt_t &pkt)	throw();
	bt_utmsg_vapi_t * utmsg_vapi()	throw()	{ return utmsg_fstart;	}

	/*************** List of friend class	*******************************/
	friend class	bt_utmsg_fstart_cnx_wikidbg_t;
	friend class	bt_utmsg_fstart_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_FSTART_CNX_HPP__  */



