/*! \file
    \brief Declaration of the neoip_slay_tls_dfull
*/


#ifndef __NEOIP_SLAY_FULL_BTJAMRC4_HPP__
#define __NEOIP_SLAY_FULL_BTJAMRC4_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_full_btjamrc4_wikidbg.hpp"
#include "neoip_slay_helper_btjamrc4.hpp"
#include "neoip_slay_full_vapi.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_jamrc4_full_t;

/** \brief class for the \ref slay_full_t of the slay_domain_t::BTJAMRC4
 */
class slay_full_btjamrc4_t : NEOIP_COPY_CTOR_DENY, public slay_full_vapi_t 
			, private wikidbg_obj_t<slay_full_btjamrc4_t, slay_full_btjamrc4_wikidbg_init>
			{
private:
	bt_jamrc4_full_t *	m_btjamrc4_full;		//!< tls context for full
public:
	/*************** ctor/dtor	***************************************/
	slay_full_btjamrc4_t()				throw();
	~slay_full_btjamrc4_t()				throw();

	/*************** Setup Function	***************************************/
	slay_err_t	start(bt_jamrc4_full_t *p_btjamrc4_full)	throw();

	/*************** Query function	***************************************/
	const slay_domain_t &	domain()		const throw()	{ return slay_helper_btjamrc4_t::DOMAIN_VAR;}

	/*************** Action function	*******************************/
	slay_err_t	notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw(); 
	slay_err_t	notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw();
	slay_err_t	notify_gracefull_shutdown(pkt_t &pkt_out)		throw();
		
	/*************** Display function	*******************************/
	std::string		to_string()	const throw();	

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(slay_full_vapi_t, slay_full_btjamrc4_t);

	/*************** List of friend function	***********************/
	friend class	slay_full_btjamrc4_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_FULL_BTJAMRC4_HPP__ */



