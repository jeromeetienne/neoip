/*! \file
    \brief Header of the \ref casti_swarm_spos_t

*/


#ifndef __NEOIP_CASTI_SWARM_SPOS_HPP__
#define __NEOIP_CASTI_SWARM_SPOS_HPP__
/* system include */
/* local include */
#include "neoip_casti_swarm_spos_wikidbg.hpp"
#include "neoip_bt_cast_spos_arr.hpp"
#include "neoip_file_size_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_swarm_t;
class	bt_ezswarm_t;
class	bt_err_t;
class	bt_scasti_vapi_t;

/** \brief Handle the bt_cast_spos_arr_t for the casti_swarm_t
 */
class casti_swarm_spos_t : NEOIP_COPY_CTOR_DENY
		, private wikidbg_obj_t<casti_swarm_spos_t, casti_swarm_spos_wikidbg_init> {
private:
	casti_swarm_t *		m_casti_swarm;	//!< backpointer to the attached casti_swarm_t
	bt_cast_spos_arr_t	m_cast_spos_arr;

	/*************** internal function	*******************************/
	bt_ezswarm_t *		bt_ezswarm()	const throw();
	bt_scasti_vapi_t *	scasti_vapi()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	casti_swarm_spos_t()	throw();
	~casti_swarm_spos_t()	throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(casti_swarm_t *casti_swarm)	throw();

	/*************** Query function	***************************************/
	const bt_cast_spos_arr_t & cast_spos_arr()	const throw()	{ return m_cast_spos_arr;	}

	/*************** Action function	*******************************/
	void		notify_pieceq_changed()			throw();
	void		gather()				throw();

	/*************** List of friend class	*******************************/
	friend class	casti_swarm_spos_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_SWARM_SPOS_HPP__ */










