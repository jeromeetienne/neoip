/*! \file
    \brief Declaration of the neoip_slay_tls_ditor
*/


#ifndef __NEOIP_SLAY_ITOR_BTJAMRC4_HPP__
#define __NEOIP_SLAY_ITOR_BTJAMRC4_HPP__
/* system include */
/* local include */
#include "neoip_slay_itor_btjamrc4_wikidbg.hpp"
#include "neoip_slay_helper_btjamrc4.hpp"
#include "neoip_slay_itor_vapi.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_slay_profile.hpp"
#include "neoip_bt_jamrc4_itor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \brief class for the \ref slay_itor_t of the slay_domain_t::BTJAMRC4
 */
class slay_itor_btjamrc4_t : public slay_itor_vapi_t
			, private wikidbg_obj_t<slay_itor_btjamrc4_t, slay_itor_btjamrc4_wikidbg_init>
			{
private:
	slay_profile_t		m_slay_profile;	//!< the slay_profile_t for this itoronder
	bt_jamrc4_itor_t	m_btjamrc4_itor;
public:
	/*************** ctor/dtor	***************************************/
	slay_itor_btjamrc4_t()			throw();
	~slay_itor_btjamrc4_t()			throw();
	
	/*************** Setup function	***************************************/
	slay_err_t	start(const slay_itor_arg_t &itor_arg)		throw();
		
	/*************** query function	***************************************/
	const slay_domain_t &	domain()	const throw()	{ return slay_helper_btjamrc4_t::DOMAIN_VAR;	}
	const slay_profile_t &	profile()	const throw()	{ return m_slay_profile;		}
	pkt_t &			xmit_buffer()	throw()		{ return m_btjamrc4_itor.xmit_buffer();	}
	
	/*************** Action function	*******************************/
	slay_err_t		notify_recved_data(const pkt_t &pkt_inp
						, slay_full_t **slay_full_out)	throw();
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	
	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(slay_itor_vapi_t, slay_itor_btjamrc4_t);

	/*************** List of friend function	***********************/
	friend class	slay_itor_btjamrc4_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_ITOR_BTJAMRC4_HPP__ */



