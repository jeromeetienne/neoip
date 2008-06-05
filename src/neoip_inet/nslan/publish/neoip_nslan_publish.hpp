/*! \file
    \brief Declaration of the nslan_publish_t
    
*/


#ifndef __NEOIP_NSLAN_PUBLISH_HPP__ 
#define __NEOIP_NSLAN_PUBLISH_HPP__ 
/* system include */
/* local include */
#include "neoip_nslan_publish_wikidbg.hpp"
#include "neoip_nslan_rec.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nslan_peer_t;

/** \brief handle a nslan_publish_t on a given nslan_peer_t
 */
class nslan_publish_t : NEOIP_COPY_CTOR_DENY
				, private wikidbg_obj_t<nslan_publish_t, nslan_publish_wikidbg_init>
				{
private:
	nslan_peer_t *	nslan_peer;	//!< backpointer to the nslan_peer_t
	nslan_rec_t	nslan_rec;	//!< the published record inside the nslan_peer_t
public:
	/*************** ctor/dtor	***************************************/
	nslan_publish_t(nslan_peer_t *nslan_peer, const nslan_rec_t &nslan_rec)	throw();
	~nslan_publish_t()									throw();
	
	/*************** query function	***************************************/
	const nslan_rec_t &	get_record()	const throw()	{ return nslan_rec;	}

	/*************** List of friend function	***********************/
	friend	class	nslan_publish_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_NSLAN_PUBLISH_HPP__ 



