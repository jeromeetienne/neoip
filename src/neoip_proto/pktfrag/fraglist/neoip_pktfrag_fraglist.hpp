/*! \file
    \brief Header of the \ref pktfrag_fraglist_t.cpp

*/


#ifndef __NEOIP_PKTFRAG_FRAGLIST_HPP__ 
#define __NEOIP_PKTFRAG_FRAGLIST_HPP__ 

/* system include */
#include <vector>
/* local include */
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class pktfrag_t;

/** \ref class to store the fragments from \ref pktfrag_t
 * 
 * - it is used to report the pkt fragment to other layers
 */
class pktfrag_fraglist_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::vector<pkt_t>	frag_db;	//!< all the received fragments
	uint32_t		seqnb;		//!< the seqnb of this datagram (common to all fragments)

	void		set_seqnb(uint32_t seqnb)		throw();
	void		add_fragment(const pkt_t &fragment)	throw();
public:
	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return frag_db.size() == 0;	}
	size_t		size()		const throw()	{ return frag_db.size();	}
	uint32_t	get_seqnb()	const throw()	{ return seqnb;			}

	pkt_t &		operator[](int idx)	throw();
	
	/*************** display functions	*******************************/
	std::string	to_string()		const throw();
	friend std::ostream &operator << (std::ostream & os, const pktfrag_fraglist_t & fraglist)throw()
					{ return os << fraglist.to_string();	}

	/*************** List of friend class	*******************************/
	friend	class pktfrag_t;
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_PKTFRAG_FRAGLIST_HPP__  */



