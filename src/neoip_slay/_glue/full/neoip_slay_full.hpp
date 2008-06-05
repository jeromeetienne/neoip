/*! \file
    \brief Header of the slay_full_t
*/


#ifndef __NEOIP_SLAY_FULL_HPP__ 
#define __NEOIP_SLAY_FULL_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_full_wikidbg.hpp"
#include "neoip_slay_full_vapi.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the slay full
 */
class slay_full_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<slay_full_t, slay_full_wikidbg_init>
			{
private:
	/*************** slay_full_vapi_t	*******************************/
	slay_full_vapi_t *	m_full_vapi;	//!< virtual api pointing on the domain implementation
	slay_full_vapi_t *	full_vapi()	throw()		{ DBG_ASSERT(m_full_vapi); return m_full_vapi;	}
	const slay_full_vapi_t*	full_vapi()	const throw()	{ DBG_ASSERT(m_full_vapi); return m_full_vapi;	}
public:
	/*************** ctor/dtor	***************************************/
	slay_full_t() 	throw();
	~slay_full_t() 	throw();
	
	/*************** Setup Helper Function	*******************************/
	slay_err_t	start(slay_full_vapi_t *p_full_vapi)	throw();
	
	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return m_full_vapi == NULL;	}
	const slay_domain_t &	domain()	const throw()	{ return full_vapi()->domain();	}

	/*************** Action function	*******************************/
	slay_err_t	notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()	{ return full_vapi()->notify_recved_data(pkt_inp, pkt_out);	} 
	slay_err_t	notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()	{ return full_vapi()->notify_toxmit_data(pkt_inp, pkt_out);	}
	slay_err_t	notify_gracefull_shutdown(pkt_t &pkt_out)		throw()	{ return full_vapi()->notify_gracefull_shutdown(pkt_out);	}
	
	/*************** Display Function	*******************************/
	std::string	to_string()		const throw()	{ DBG_ASSERT(!is_null());
								  return full_vapi()->to_string();	}	
	friend std::ostream & operator << (std::ostream & os, const slay_full_t & full)
						throw()		{ return os << full.to_string();	}

	/*************** List of friend function	***********************/
	friend class	slay_full_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_FULL_HPP__  */



