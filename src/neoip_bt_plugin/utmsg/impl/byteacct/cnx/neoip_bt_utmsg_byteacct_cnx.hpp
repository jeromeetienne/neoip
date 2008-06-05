/*! \file
    \brief Header of the bt_utmsg_byteacct_cnx_t
    
*/


#ifndef __NEOIP_BT_UTMSG_BYTEACCT_CNX_HPP__ 
#define __NEOIP_BT_UTMSG_BYTEACCT_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_utmsg_byteacct_cnx_wikidbg.hpp"
#include "neoip_bt_utmsg_cnx_vapi.hpp"
#include "neoip_bt_utmsg_byteacct.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_file_size.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_full_utmsg_t;
class	bt_cmd_t;
class	pkt_t;
class	bt_utmsgtype_t;

/** \brief class definition for bt_utmsg_byteacct_cnx_t
 */
class bt_utmsg_byteacct_cnx_t : NEOIP_COPY_CTOR_DENY
		, public bt_utmsg_cnx_vapi_t, private timeout_cb_t
		, private wikidbg_obj_t<bt_utmsg_byteacct_cnx_t, bt_utmsg_byteacct_cnx_wikidbg_init, timeout_cb_t, bt_utmsg_cnx_vapi_t>
		{
private:
	bt_utmsg_byteacct_t *	utmsg_byteacct;	//!< backpointer on the bt_utmsg_byteacct_t
	bt_swarm_full_utmsg_t *	full_utmsg;	//!< backpointer on the bt_swarm_full_utmsg_t
	file_size_t		m_dloaded_len;	//!< the last reported dloaded_len
	file_size_t		m_uloaded_len;	//!< the last reported uloaded_len
	
	/*************** rate estimation	*******************************/
	double			m_recv_rate;
	double			m_xmit_rate;
	date_t			last_update;	//!< TODO find a better name

	/*************** internal function	*******************************/
	bt_cmd_t		generate_xmit_cmd()				throw();
	bool			parsing_error(const std::string &reason)	throw();

	/*************** xmit_timeout_t	***************************************/
	delaygen_t		xmit_delaygen;	//!< the delay_t generator for xmit	
	timeout_t		xmit_timeout;	//!< to periodically send a packet
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_byteacct_cnx_t() 		throw();
	~bt_utmsg_byteacct_cnx_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(bt_utmsg_byteacct_t *utmsg_byteacct
					, bt_swarm_full_utmsg_t *m_full_utmsg)	throw();	

	/*************** Query function	***************************************/
	const file_size_t &	dloaded_len()	const throw()	{ return m_dloaded_len;	}
	const file_size_t &	uloaded_len()	const throw()	{ return m_uloaded_len;	}
	const double &		recv_rate()	const throw()	{ return m_recv_rate;	}
	const double &		xmit_rate()	const throw()	{ return m_xmit_rate;	}
	bool			rate_available()const throw()	{ return recv_rate() != std::numeric_limits<double>::max();	}

	/*************** bt_utmsg_cnx_vapi_t	*******************************/
	bool			parse_pkt(const bt_utmsgtype_t &bt_utmsgtype, const pkt_t &pkt)	throw();
	bt_utmsg_vapi_t *	utmsg_vapi()	throw()	{ return utmsg_byteacct;	}

	/*************** List of friend class	*******************************/
	friend class	bt_utmsg_byteacct_cnx_wikidbg_t;
	friend class	bt_utmsg_byteacct_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_BYTEACCT_CNX_HPP__  */



