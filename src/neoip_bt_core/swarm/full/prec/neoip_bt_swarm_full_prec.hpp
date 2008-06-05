/*! \file
    \brief Header of the bt_swarm_full_t
    
*/


#ifndef __NEOIP_BT_SWARM_FULL_PREC_HPP__ 
#define __NEOIP_BT_SWARM_FULL_PREC_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_swarm_full_prec_wikidbg.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for bt_swarm_full_prec_t
 */
class bt_swarm_full_prec_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_swarm_full_prec_t, bt_swarm_full_prec_wikidbg_init>
			{
private:
	bt_swarm_full_t *	swarm_full;	//!< the bt_swarm_full_t to which it is attached

	/*************** precedence for the bt_reqauth_type_t	***************/
	double			m_reqauth_prec_xmit;
	double			m_reqauth_prec_recv;
	
	/*************** Internal function	*******************************/
	void			update_prec_xmit()		throw();
	void			update_prec_recv()		throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_full_prec_t(bt_swarm_full_t *swarm_full) 	throw();
	~bt_swarm_full_prec_t()					throw();
	
	/*************** Query function	***************************************/
	rate_prec_t	rate_prec_xmit()	const throw();
	rate_prec_t	rate_prec_recv()	const throw();

	/*************** sub precedence	***************************************/
	double	reqauth_prec_xmit()		const throw()	{ return m_reqauth_prec_xmit;	}
	void	reqauth_prec_xmit(double new_val)	throw()	{ DBG_ASSERT( new_val >= 0.0 && new_val <= 1.0 );
								  m_reqauth_prec_xmit = new_val;
								  update_prec_xmit();		}
	double	reqauth_prec_recv()		const throw()	{ return m_reqauth_prec_recv;	}
	void	reqauth_prec_recv(double new_val)	throw()	{ DBG_ASSERT( new_val >= 0.0 && new_val <= 1.0 );
								  m_reqauth_prec_recv = new_val;
								  update_prec_recv();		}

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_full_prec_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_FULL_PREC_HPP__  */



