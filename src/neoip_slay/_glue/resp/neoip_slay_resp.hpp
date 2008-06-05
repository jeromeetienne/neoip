/*! \file
    \brief Header of the slay_resp_t
*/


#ifndef __NEOIP_SLAY_RESP_HPP__ 
#define __NEOIP_SLAY_RESP_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_resp_wikidbg.hpp"
#include "neoip_slay_resp_vapi.hpp"
#include "neoip_slay_resp_arg.hpp"
#include "neoip_slay_domain.hpp"
#include "neoip_slay_profile.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the slay resp
 */
class slay_resp_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<slay_resp_t, slay_resp_wikidbg_init>
			{
private:
	/*************** slay_resp_vapi_t	*******************************/
	slay_resp_vapi_t *	m_resp_vapi;	//!< virtual api pointing on the domain implementation
	slay_resp_vapi_t *	resp_vapi()	throw()		{ DBG_ASSERT(m_resp_vapi); return m_resp_vapi;	}
	const slay_resp_vapi_t*	resp_vapi()	const throw()	{ DBG_ASSERT(m_resp_vapi); return m_resp_vapi;	}
public:
	/*************** ctor/dtor	***************************************/
	slay_resp_t() 	throw();
	~slay_resp_t() 	throw();
	
	/*************** Setup Helper Function	*******************************/
	slay_err_t	start(const slay_resp_arg_t &arg)	throw();
	
	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return m_resp_vapi == NULL;		}
	const slay_domain_t &	domain()	const throw()	{ return resp_vapi()->domain();		}
	const slay_profile_t &	profile()	const throw()	{ return resp_vapi()->profile();	}
	pkt_t &			xmit_buffer()	throw()		{ return resp_vapi()->xmit_buffer();	}

	/*************** Action function	*******************************/
	slay_err_t	notify_recved_data(const pkt_t &pkt_inp, slay_full_t **slay_full_out)	throw()
						{ return resp_vapi()->notify_recved_data(pkt_inp, slay_full_out);	} 

	/*************** Display Function	*******************************/
	std::string	to_string()		const throw()	{ DBG_ASSERT(!is_null());
								  return resp_vapi()->to_string();	}	
	friend std::ostream & operator << (std::ostream & os, const slay_resp_t & resp)
						throw()		{ return os << resp.to_string();	}

	/*************** List of friend function	***********************/
	friend class	slay_resp_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_RESP_HPP__  */



