/*! \file
    \brief Header of the slay_itor_t
*/


#ifndef __NEOIP_SLAY_ITOR_HPP__ 
#define __NEOIP_SLAY_ITOR_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_itor_wikidbg.hpp"
#include "neoip_slay_itor_vapi.hpp"
#include "neoip_slay_itor_arg.hpp"
#include "neoip_slay_domain.hpp"
#include "neoip_slay_profile.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the slay itor
 */
class slay_itor_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<slay_itor_t, slay_itor_wikidbg_init>
			{
private:
	/*************** slay_itor_vapi_t	*******************************/
	slay_itor_vapi_t *	m_itor_vapi;	//!< virtual api pointing on the domain implementation
	slay_itor_vapi_t *	itor_vapi()	throw()		{ DBG_ASSERT(m_itor_vapi); return m_itor_vapi;	}
	const slay_itor_vapi_t*	itor_vapi()	const throw()	{ DBG_ASSERT(m_itor_vapi); return m_itor_vapi;	}
public:
	/*************** ctor/dtor	***************************************/
	slay_itor_t() 	throw();
	~slay_itor_t() 	throw();
	
	/*************** Setup Helper Function	*******************************/
	slay_err_t	start(const slay_itor_arg_t &arg)	throw();
	
	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return m_itor_vapi == NULL;		}
	const slay_domain_t &	domain()	const throw()	{ return itor_vapi()->domain();		}
	const slay_profile_t &	profile()	const throw()	{ return itor_vapi()->profile();	}
	pkt_t &			xmit_buffer()	throw()		{ return itor_vapi()->xmit_buffer();	}

	/*************** Action function	*******************************/
	slay_err_t	notify_recved_data(const pkt_t &pkt_inp, slay_full_t **slay_full_out)	throw()
						{ return itor_vapi()->notify_recved_data(pkt_inp, slay_full_out);	} 

	/*************** Display Function	*******************************/
	std::string	to_string()		const throw()	{ DBG_ASSERT(!is_null());
								  return itor_vapi()->to_string();	}	
	friend std::ostream & operator << (std::ostream & os, const slay_itor_t & itor)
						throw()		{ return os << itor.to_string();	}

	/*************** List of friend function	***********************/
	friend class	slay_itor_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_ITOR_HPP__  */



