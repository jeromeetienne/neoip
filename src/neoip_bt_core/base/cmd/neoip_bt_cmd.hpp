/*! \file
    \brief Header of the \ref bt_cmd_t
    
*/


#ifndef __NEOIP_BT_CMD_HPP__ 
#define __NEOIP_BT_CMD_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_cmdtype.hpp"
#include "neoip_bt_prange.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;

/** \brief class definition for bt_cmd_t
 */
class bt_cmd_t : NEOIP_COPY_CTOR_ALLOW {
private:
	bt_cmdtype_t	cmdtype_val;
	size_t		piece_idx_val;
	bt_prange_t	prange_val;
	datum_t		utmsg_datum_val;
public:
	/*************** ctor/dtor	***************************************/
	bt_cmd_t()	throw();

	/*************** query function	***************************************/
	bool			is_null()	const throw()	{ return cmdtype().is_null();	}
	const bt_cmdtype_t &	cmdtype()	const throw()	{ return cmdtype_val;		}
	size_t			piece_idx()	const throw()	{ DBG_ASSERT(cmdtype() == bt_cmdtype_t::PIECE_ISAVAIL);
								  return piece_idx_val;		}
	const bt_prange_t &	prange()	const throw()	{ DBG_ASSERT(cmdtype() == bt_cmdtype_t::BLOCK_REQ
									|| cmdtype() == bt_cmdtype_t::BLOCK_REP
									|| cmdtype() == bt_cmdtype_t::BLOCK_DEL );
								  return prange_val;	}
	const datum_t &		utmsg_datum()	const throw()	{ DBG_ASSERT(cmdtype() == bt_cmdtype_t::UTMSG_PAYL);
								  return utmsg_datum_val;	}
	size_t			get_datum_len(const bt_swarm_t &bt_swarm)	const throw();
	datum_t			to_datum(const bt_swarm_t &bt_swarm)		const throw();

	/*************** Comparison function	*******************************/
	int	compare(const bt_cmd_t &other)	     const throw();
	bool 	operator == (const bt_cmd_t & other) const throw() { return compare(other) == 0;	}
	bool 	operator != (const bt_cmd_t & other) const throw() { return compare(other) != 0;	}
	bool 	operator <  (const bt_cmd_t & other) const throw() { return compare(other) <  0;	}
	bool 	operator <= (const bt_cmd_t & other) const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const bt_cmd_t & other) const throw() { return compare(other) >  0;	}
	bool 	operator >= (const bt_cmd_t & other) const throw() { return compare(other) >= 0;	}

	/*************** build function	***************************************/
	static bt_cmd_t	build_keepalive()					throw();
	static bt_cmd_t	build_unauth_req()					throw();
	static bt_cmd_t	build_doauth_req()					throw();
	static bt_cmd_t	build_dowant_req()					throw();
	static bt_cmd_t	build_unwant_req()					throw();
	static bt_cmd_t	build_piece_isavail(size_t piece_idx)			throw();
	static bt_cmd_t	build_piece_bfield()					throw();
	static bt_cmd_t	build_block_req(const bt_prange_t &bt_prange)		throw();
	static bt_cmd_t	build_block_rep(const bt_prange_t &bt_prange)		throw();
	static bt_cmd_t	build_block_del(const bt_prange_t &bt_prange)		throw();
	static bt_cmd_t	build_utmsg_payl(const datum_t &payload)		throw();

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_cmd_t &bt_cmd)	throw()
				{ return os << bt_cmd.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CMD_HPP__  */










