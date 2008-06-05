/*! \file
    \brief Header of the bt_utmsg_piecewish_cnx_t
    
*/


#ifndef __NEOIP_BT_UTMSG_PIECEWISH_CNX_HPP__ 
#define __NEOIP_BT_UTMSG_PIECEWISH_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_utmsg_piecewish_cnx_wikidbg.hpp"
#include "neoip_bt_utmsg_cnx_vapi.hpp"
#include "neoip_bt_utmsg_piecewish.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_full_utmsg_t;
class	bt_cmd_t;
class	bt_utmsgtype_t;
class	pkt_t;
class	bitfield_t;
class	dvar_arr_t;

/** \brief class definition for bt_utmsg_piecewish_cnx_t
 */
class bt_utmsg_piecewish_cnx_t : NEOIP_COPY_CTOR_DENY, public bt_utmsg_cnx_vapi_t
		, private wikidbg_obj_t<bt_utmsg_piecewish_cnx_t, bt_utmsg_piecewish_cnx_wikidbg_init, bt_utmsg_cnx_vapi_t>
		{
private:
	bt_utmsg_piecewish_t *	utmsg_piecewish;//!< backpointer on the bt_utmsg_piecewish_t
	bt_swarm_full_utmsg_t *	full_utmsg;	//!< backpointer on the bt_swarm_full_utmsg_t
	
	/*************** command parsing	*******************************/
	bool	parsing_error(const std::string &reason)		throw();
	bool	parse_dowish_field(const std::string &field_str)	throw();
	bool	parse_dowish_index(const dvar_arr_t &dvar_arr)		throw();
	bool	parse_nowish_index(const dvar_arr_t &dvar_arr)		throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_piecewish_cnx_t() 		throw();
	~bt_utmsg_piecewish_cnx_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(bt_utmsg_piecewish_t *utmsg_piecewish
					, bt_swarm_full_utmsg_t *m_full_utmsg)	throw();	

	/*************** Action function	*******************************/
	void			send_cmd(const bt_cmd_t &bt_cmd)	throw();

	/*************** bt_utmsg_cnx_vapi_t	*******************************/
	bool			parse_pkt(const bt_utmsgtype_t &bt_utmsgtype, const pkt_t &pkt)	throw();
	bt_utmsg_vapi_t *	utmsg_vapi()	throw()	{ return utmsg_piecewish;	}

	/*************** List of friend class	*******************************/
	friend class	bt_utmsg_piecewish_cnx_wikidbg_t;
	friend class	bt_utmsg_piecewish_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_PIECEWISH_CNX_HPP__  */



